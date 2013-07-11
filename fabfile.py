from fabric.api import *
import os.path
import shutil
import sys

#should be command line arguments
#to deploy, run the following
#
# $ fab -H uxcptln111 deploy
#
 

gbl_branch = "master"
gbl_release_path = '/tmp/edrmake_p/releases/'
gbl_current = '/tmp/edrmake_p/current'
env.user = 'hbehrens'

def git_clone():
	result = local('git ls-remote file:///c/users/hartmut.behrens/workspace/edrmake/.git '+gbl_branch, capture=True)
	git_ref = get_git_ref();
	clone_path = 'temp/'+git_ref
	local('git clone -q -b master .git '+clone_path)
	shutil.rmtree(clone_path+'/.git')
	
def copy():
	git_ref = get_git_ref()
	run('mkdir -p -m 777 %s' %gbl_release_path)
	run('chmod -R ugo+rw %s' %gbl_release_path)
	put('temp/'+git_ref,gbl_release_path)
	
def make():
	git_ref = get_git_ref()
	releasepath = gbl_release_path + git_ref
	with cd(releasepath):
		run('make clean')
		run('make')
		
def link():
	git_ref = get_git_ref()
	releasepath = gbl_release_path + git_ref
	run('rm -rf %s' % gbl_current)
	run('ln -s %s %s' % (releasepath, gbl_current))
	
def deploy():
	git_ref = get_git_ref();
	clone_path = 'temp/'+git_ref
	if not os.path.exists(clone_path):
		git_clone()
	copy()
	make()
	link()
	
def get_git_ref():
	global gbl_repo, gbl_branch
	result = local('git ls-remote file:///c/users/hartmut.behrens/workspace/edrmake/.git '+gbl_branch, capture=True)
	words = result.split()
	return words[0]
	
def cleanup():
	path = 'temp/'
	if os.path.exists(path):
		shutil.rmtree(path)
	
