# LHCbSoftware on Openstack

## Motivation

The easiest way to to work in an LHCb Software Evironment is by logging in to an `lxplus` instance.
From there one also has access to the [`AFS` file storage](http://information-technology.web.cern.ch/services/afs-service).

However, working on `lxplus` comes with a few significant limitations:
 * No super user access. This makes it hard to install new software.
 * Shared resources with other users. Others might start resource-hungry processes on the same machine, therefore extending execution time and making it unpredicatable.

Using a virtual machine on [`openstack.cern.ch`](https://openstack.cern.ch) solves these issues. It is possible to provision a machine with up to 10 vCPUs and up to 20 GB memory. (There are user quotas on total the number of instances, RAM and vCPUs used.) While  vCPUs are not the same as physical CPUs, using a virtual machine comes with with a significant improvement in performance and performance reliablity. There are no limits on time or memory for running processes. 


This tutorial shows how one can easily deploy a machine on Openstack which offers the LHCb software environment and AFS access, using the CernVM virtual machine image.You will be able to deploy a powerful machine on openstack, run LHCb Software there while enjoying full super user access and access to AFS. You can take snapshots of your machines and if all goes awry, you can choose the nuclear option on your machine - and set up a new one.


This can be done using the command line only. No step in the tutorial requires any GUI interaction. Here is what we will do:
 * Register with openstack and set up the necessary authentication.
 * Make sure that a special CernVM virtual machine image is avaiable.
 * Start the CernVM virtual machine with special start parameters that configure our environment.
 * Log in to change the password and authenticate for AFS access.



## Set up an Openstack instance with AFS and the LHCb Software environment


This is a very basic tutorial, some steps described can be skipped if you have already done them.


### Register for [OpenStack](https://openstack.cern.ch). 
The way to register is described [here](https://clouddocs.web.cern.ch/clouddocs/tutorial_using_a_browser/subscribe_to_the_cloud_service.html) in the [OpenStack documentation](https://clouddocs.web.cern.ch/clouddocs/)

### Add public key to Openstack
Log in to lxplus,  create an ssh key-pair. Add your public key to to your [list of public keys on openstack](https://openstack.cern.ch/project/access_and_security/)


### Download `openrc.sh`
Download your personal OpenStack RC file. 
You can find it on https://openstack.cern.ch/project/access_and_security/ if you click on the "API Access" index tab.  
Save it as `openrc.sh`.

### Set up your environment

```
source ~/openrc.sh
export OS_CACERT=/etc/pki/tls/cert.pem
```
    
### Upload [CERNVM](https://cernvm.cern.ch/) image

The default VM images do not have the LHCb Software Environment installed, nor do they have access to AFS.
Upload the CERNVM image to OpenStack (details [here](https://cernvm.cern.ch/portal/openstack) ) 
    
```
wget http://cernvm.cern.ch/releases/production/cernvm-3.5.1.hdd
glance image-create --name "CernVM 3" --is-public False --disk-format raw \
--property os=linux --property hypervisor_type=kvm --container-format bare \
--file cernvm-3.5.1.hdd
```

You can (and maybe should) replace `cernvm-3.5.1.hdd` with a newer version of CernVM here. Have a look [here](https://cernvm.cern.ch/releases/production/) for possible downloads.

The response should be something like this:
```
+----------------------------+--------------------------------------+
| Property                   | Value                                |
+----------------------------+--------------------------------------+
| Property 'hypervisor_type' | kvm                                  |
| Property 'os'              | linux                                |
| checksum                   | bc95d2486e7991a99f40dc371d855f58     |
| container_format           | bare                                 |
| created_at                 | 2016-03-16T11:32:37.000000           |
| deleted                    | False                                |
| deleted_at                 | None                                 |
| disk_format                | raw                                  |
| id                         | 3d46fbda-97ba-4c8b-a48b-f2e3c3e418b3 |
| is_public                  | False                                |
| min_disk                   | 0                                    |
| min_ram                    | 0                                    |
| name                       | CernVM 3                             |
| owner                      | 921dfe9a-9396-4d1b-aa9d-9c7ebd963235 |
| protected                  | False                                |
| size                       | 21037056                             |
| status                     | active                               |
| updated_at                 | 2016-03-16T11:32:42.000000           |
| virtual_size               | None                                 |
+----------------------------+--------------------------------------+
```
    
This confirms that you have uploaded the CernVM image to Openstack. 
You can see it in the web interface in [your list of images](https://openstack.cern.ch/project/images/).

### Define CernVM config

Copy the following lines in a file. We will refer to this file as `<context-config.txt>`.
It is a CernVM Contextualization file. [Here is some documentation](https://cernvm.cern.ch/portal/contextualisation) and 
[here you can configure your own contexts](https://cernvm-online.cern.ch/dashboard/).

```
#!/bin/sh
. /etc/cernvm/site.conf
mkdir /afs
/sbin/chkconfig afs on
/sbin/service afs start
wget http://linuxsoft.cern.ch/cern/slc55/i386/yum/updates/useraddcern-0.3-1.slc5.noarch.rpm
yum -y install useraddcern-0.3-1.slc5.noarch.rpm
[amiconfig]
plugins=cernvm

[cernvm]
organisations=LHCB
repositories=lhcb,grid
shell=/bin/bash
config_url=http://cernvm.cern.ch/config
users=default:lhcb:$6$BkNNAXkW$9ToClG4fU6Srqn8MVKJ2TA3k4j0/wXTkSxlOK4il7FhRXsIbSvylCevXmmccTBnYJsxtnIh9kzUEtQbP1g23.0
edition=Batch

[ucernvm-begin]
cvmfs_branch=cernvm-prod.cern.ch
[ucernvm-end]
```

Start the instance with 
```bash
nova boot <host-name>  --image "CernVM 3" --flavor m2.large --key-name <key-name>  --user-data <context-config.txt>
```

Here, you need to replace `<host-name>` with a name of your choice 
and `<key-name>` with the name of a public key which you uploaded to OpenStack. 

Use at least m1.small as a flavor, the m1.tiny flavor is too small. 

The output will look something like this:

```
+--------------------------------------+-------------------------------------------------+
| Property                             | Value                                           |
+--------------------------------------+-------------------------------------------------+
| OS-DCF:diskConfig                    | MANUAL                                          |
| OS-EXT-AZ:availability_zone          | nova                                            |
| OS-EXT-STS:power_state               | 0                                               |
| OS-EXT-STS:task_state                | scheduling                                      |
| OS-EXT-STS:vm_state                  | building                                        |
| OS-SRV-USG:launched_at               | -                                               |
| OS-SRV-USG:terminated_at             | -                                               |
| accessIPv4                           |                                                 |
| accessIPv6                           |                                                 |
| config_drive                         |                                                 |
| created                              | 2016-03-16T13:25:41Z                            |
| flavor                               | m2.large (12076)                                |
| hostId                               |                                                 |
| id                                   | 335994c0-7e06-4423-afd3-5a21836b0cca            |
| image                                | CernVM 3 (3d46fbda-97ba-4c8b-a48b-f2e3c3e418b3) |
| key_name                             | CERN                                            |
| metadata                             | {}                                              |
| name                                 | valgrinder                                      |
| os-extended-volumes:volumes_attached | []                                              |
| progress                             | 0                                               |
| security_groups                      | default                                         |
| status                               | BUILD                                           |
| tenant_id                            | 921dfe9a-9396-4d1b-aa9d-9c7ebd963235            |
| updated                              | 2016-03-16T13:25:41Z                            |
| user_id                              | apuignav                                        |
+--------------------------------------+-------------------------------------------------+
```


It takes about 10 minutes until the machine can be reached via ssh. 
You can query the status of the instance via  
```bash
nova show <host-name>
```

As soon networking is done and the machine has booted, you can log in via

```
ssh default@<host-name>
```
The `default` user has password `testtest`. You should change it after you log in.

You can also log in as root, but the LHCbSoftware will not be available
```
ssh root@<host-name>
```
The key which you specified in the `nova boot` command is the key which is used by `ssh` for authentication as the `root` user.


### Set up AFS
In order to use afs, you need to authenticate with keberos after login:
```
kinit <your-cern-username>@CERN.CH
aklog
```

## Conclusions

The tutorial has shown how to deploy a machine on Openstack which offers the LHCb software environment and AFS access. The machine is set up using the CernVM virtual machine image with a lhcb-specific configuration. This means that one can run and test LHCb software on a powerful machine with super user access and AFS mount.
