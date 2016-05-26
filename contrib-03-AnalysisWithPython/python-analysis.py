# coding: utf-8
import root_pandas

def get_list_branches(root_file, directory='', tree='DecayTree'):
    '''function to return the list of branches in a TTree in pandas.
    arguments :
        root_file : the name of the file, can be eos://...
        directory: the name of the directory or even path/to/TTree
        tree : name of the TTree
    '''
    if not directory:
        df = root_pandas.read_root(root_file, stop=1, key='{0}'.format(tree))
    else : 
        df = root_pandas.read_root(root_file, stop=1, key='{0}/{1}'.format(directory, tree))
    return df.columns
