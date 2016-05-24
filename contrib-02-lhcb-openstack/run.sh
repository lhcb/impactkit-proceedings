source ~/openrc.sh 
export OS_CACERT=/etc/pki/tls/cert.pem
python make-lhcb-openstack.py $*
