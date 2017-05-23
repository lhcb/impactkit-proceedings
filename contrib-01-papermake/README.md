# Papermake

Papermake takes variables from pickle files (in `pickles/`) and inserts them into TeX files. To use:

    papermake myfile.tex

which will create a file named `build/myfile.processed.tex`. For more information on how to use papermake, type:

    papermake --help
    usage: papermake [-h] [--build-dir BUILD_DIR] [--pickle-dir PICKLE_DIR]
                     texfile [texfile ...]
    
    positional arguments:
      texfile               TeX files to parse.
    
    optional arguments:
      -h, --help            show this help message and exit
      --build-dir BUILD_DIR
                            Build directory for tex files.
      --pickle-dir PICKLE_DIR
                            Pickle directory for pickle files.

