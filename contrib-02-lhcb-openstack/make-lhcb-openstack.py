__author__ = "Konstantin Schubert"
__email__ = "Konstantin@schubert.fr"

import argparse
import subprocess
import time
import getpass
import os

"""
	########################################
	IF YOU WANT TO CALL THIS FILE DIRECTLY, YOU MUST CALL ~/
	Use the ./run.sh wrapper script which takes care of setting up the right environment variables
	######################################
	Prerequisites:
	## Set up an Openstack instance with AFS and the LHCb Software environment

	There [is a script](...LINK...) which does all of this automatically, providing that you have registered with OpenStack, uploaded your public key and downloaded and saved your `openrc.sh` in `~`


	This is a very basic tutorial, some steps described can be skipped if you have already done them.


	### Register for [OpenStack](https://openstack.cern.ch). 
	The way to register is described [here](https://clouddocs.web.cern.ch/clouddocs/tutorial_using_a_browser/subscribe_to_the_cloud_service.html) in the [OpenStack documentation](https://clouddocs.web.cern.ch/clouddocs/)

	### Add public key to Openstack
	Log in to lxplus,  create an ssh key-pair. Add your public key to to your [list of public keys on openstack](https://openstack.cern.ch/project/access_and_security/)


	### Download `openrc.sh`
	Download your personal OpenStack RC file. 
	You can find it on https://openstack.cern.ch/project/access_and_security/ if you click on the "API Access" index tab.  
	Save it as `openrc.sh`.
"""

setup_environment = "source ~/openrc.sh && export OS_CACERT=/etc/pki/tls/cert.pem"

def create_image():
	if "CernVM 3" in subprocess.check_output("nova image-list", shell=True):
		print("INFO: Image CERNVM 3 already exists")
		return
	subprocess.check_output("wget http://cernvm.cern.ch/releases/production/cernvm-3.5.1.hdd", shell=True)
	subprocess.check_output("glance image-create --name \"CernVM 3\" --is-public False --disk-format raw --property os=linux --property hypervisor_type=kvm --container-format bare --file cernvm-3.5.1.hdd", shell=True)

def start_instance():
	output = subprocess.check_output("nova boot {host_name}  --image \"CernVM 3\" --flavor {instance_flavor} --key-name {ssh_pub_key}  --user-data context-config.txt".format( 
						instance_flavor=args.instance_flavor, ssh_pub_key=args.ssh_pub_key, host_name=args.host_name), shell=True)
	print output
	while("networking" in output or "building" in output):
		print "networking... takes about 10 minutes"
		time.sleep(20)
		output = subprocess.check_output("nova show {host_name}".format(host_name=args.host_name), shell=True)
	print output
	ssh_works = False
	while(not ssh_works):
		print "waiting for ssh ... takes a minute or two"
		time.sleep(10)
		try:
			subprocess.check_output("ssh -o \"StrictHostKeyChecking no\" root@{host_name} exit".format(host_name=args.host_name), shell=True)
			ssh_works = True
		except:
			pass
	print "ssh available"

def add_cern_user():
	print(subprocess.check_output("ssh -o \"StrictHostKeyChecking no\" root@{host_name} useraddcern koschube".format(host_name=args.host_name) , shell=True))


parser = argparse.ArgumentParser()
parser.add_argument("host_name", help="Host name of the image that will be created")
parser.add_argument("ssh_pub_key", help="The openstack-name of an ssh key which was added to openstack.cern.ch "
													  "and should be installed on the machine which runs this script")
parser.add_argument("cern_user", help="Your lxplus username.")
parser.add_argument("--flavor", dest="instance_flavor", default="m2.medium", help="The flavor (size) of the instance you want to create. Default is m2.medium")
parser.add_argument("--openrc", dest="openrc_path", default="./openrc.sh", help="The path to your openrc.sh file. More info: https://gitlab.cern.ch/koschube/lhcb-software-openstack#download-openrcsh")

args = parser.parse_args()

create_image()
start_instance()
add_cern_user()
try:
	os.remove("config.make-lhcb-openstack")
except:
	pass
try:
	os.remove("cernvm-3.5.1.hdd")
except:
	pass


message = """
You can now log in with 
ssh default@{host_name}
and password 
testtest
.

Once you are logged in, 
change the password with
passwd default
and call
kinit {cern_user}@CERN.CH
aklog
to gain AFS access.

You can also log in as root:
ssh root@{host_name}
but then the LHCb Software won't be available.

-----------------------------------------

There is also a user called {cern_user}, which was added with the command useraddcern. 
However, 

(1) when you log in via "ssh {cern_user}@{host_name}" first, the AFS-authentication seems fail
in some way. You have to call kinit {cern_user}@CERN.CH, then call aklog, then log out and log in again.
If you just call aklog, this gives a strange error and one cannot log back in again
Maybe this is a hint on what's going wrong?
You also must enter your password every time you want to ssh into the instance, even if you obtained a ticket on lxplus


(2) the LHCb Software seems to be unavailable.

If you know how to fix (1) or (2) let me know at
https://gitlab.cern.ch/koschube/lhcb-software-openstack
or via email to 
konstantin@schubert.fr
""".format(host_name=args.host_name, cern_user=args.cern_user)

print(message)

"""
to fix (1)
in /etc/ssh/sshd_config, set 
KerberosAuthentication yes
KerberosGetAFSToken yes


to fix (2) log in once with {cern_user} and call kinit koschube@CERN.CH, then aklog, then log out and log in again
If you just call aklog, this gives a strange error and one cannot log back in again
maybe this is a hint on what's going wrong?

"""
