# Getting started with python-based analysis 

## Motivation

In High Energy Physics, almost every data analysis framework involved the ROOT
software. Built in the 90's, it has been very useful to perform all the tasks
required by an analysis, from the handle of large datasets to the use of
powerful statistical procedures with the help of librairies such as RooFit or
TMVA.  
However, it hasn't spread outside of the HEP community, while other projects
also dedicated to data analysis are now very mature with a much larger
users community.
The goal of this project was to build an analysis demonstrator capable of :
- use the rich environment of python libraries to perform analysis tasks
- load and store processed data in ROOT file format, in order to be able to
  fully collaborate with ROOT users.

## Environment setup

In order to install and load all the packages we need for this project, we rely
on the [anaconda python distribution](https://www.continuum.io/downloads).

We tested the following environment on lxplus :

- Install [miniconda for python2.7, Linux 64 bit](http://conda.pydata.org/miniconda.html) via `bash
  Miniconda2-latest_Linux-x86_64.sh` and follow the instructions given by the
  prompt. If you prefer a more complete distribution, you can install
  [anaconda](https://www.continuum.io/downloads)
  instead following the same procedure.

- A non-mandatory step would be to create a virtual environment to keep the
  local python distribution as clean as possible. It can be achieve trough
  :
    - `conda create -n NameOfYourVirtualEnv python=2.7 anaconda` to create it.
    - `source activate NameOfYourVirtualEnv` to activate it
    - `source deactivate` to go back to normal 

- Install ROOT inside the conda environment : `conda install -c chrisburr root=6.06.02`

- You will then need to activate the conda environment :
    - `source activate root` to load the main conda environment
    - `source activate NameOfYourVirtualEnv` if you made it yourself

- A few packages are also needed and can be installed using pip, the python
  package manager :
    - [Numpy](http://www.numpy.org/) (arrays and tools for scientific computing), [Matplotlib](http://matplotlib.org/) (plotting
      library), [scikit-learn](http://scikit-learn.org/stable/) (Machine Learning in Python) :
      `pip install numpy matplotlib scikit-learn`
    - The [root_pandas package](https://github.com/ibab/root_pandas) is based on   [root_numpy](https://github.com/rootpy/root_numpy) and allows you to load a ROOT
      file and store the content in a pandas dataframe. Follow both links if
      you want more information on how to load a ROOT file. To install it, do
      `pip install root_pandas`
    - In order to produce a [jupyter](http://jupyter.org/) notebook which is a way to keep both the
      analysis code and its output in the same file, you can also do `pip install jupyter`. Then, to load a jupyter notebook, you can do
      `jupyter notebook` and access the notebook in your browser.

## Loading data

Using the root_pandas library, we can load a ROOT file in a few simple steps :
- choose the file you want and store the path as string.
- then select the branches you want by creating a list with the names of the
  variables you want. The use of of the wildcard `*` is also supported. 
- As the file will be loaded in the memory, you should limit to a certain
  number of entries and if you really need to load each event, the
  [documentation](https://github.com/ibab/root_pandas) of the package is worth to read.
- Finally, select the name of your tuple.

```python
# Path of the ROOT file
myfile = "path/to/file.root"

# Branches to load, wildcards allowed
mybranches = ["B0_M", "*_PT"]

# Number of entries to load
n_events = 10000

# Name of the tree
tuple_name = "someDir/myTree"
```

Then, you will be allowed to store your tree as a pandas dataframe using :
```python
   import root_pandas
   df = root_pandas.read_root(myfile, columns=mybranches, stop=n_events, key=tuple_name)
```
## Creating and plotting histograms

To produce a simple plot, let's say, the mass of the $B^{0}$, you can simply do :
```python
   import numpy as np
   custom_binning = np.linspace(5000,5500,100) #min, max and number of bins 
   mass_B0 = df['B0_M'].plot.hist(bins=custom_binning, label = 'mass of the
   B^{0}_{M} [MeV/c$^{2}$]'} #label of the plot can be written in LaTeX
```

Then, you just need to add `plt.show()` in your code to plot the histogram.
A more detailed code is written in both `compare_mc_data.py` and
`AnalysisWithPython.ipynb`.  


## Selecting events 

One needs to cut either to reduce a dataset or to produce some plot and there
are two way do this :
- *create a reduced dataset* using the `query` method of pandas : each event of
  the initial dataset passing the selection will be copied to create a new
  dataset. It will cost more memory as it duplicates selected events but in the
  case of a really large dataset, it can be useful to create a reduced dataset
  before performing other analysis tasks.
- *selecting events within the current dataset* using a mask is a more economical
  way to handle memory but one needs to be careful about the changes made to
  the reduced dataset as they will be propagated to the initial one. 


```python
    #Create a new reduced dataset
    cut_string = " B0_M >5250 & B0_P > 500 "
    df_reduced_with_copy = df.query(cut_string)

    #Select events within the current dataset
    mask = (df['B0_M'] > 5250) & (df['B0_P'] > 500)
    df_reduced_without_copy = df[mask]
```  


Which method to choose will depend on the use-case. And then, the plotting part
is taken care by :  
    ```python
        df.query(cut_string)['B0_M'].plot.hist(bins=custom_binning)
    ```

## MVA

ROOT users are used to TMVA when they need to train a BDT for their analysis.
The [scikit-learn](http://scikit-learn.org/stable/) project is the most mature way to such task in Python.
Posts from Tim Head's
[blog](http://betatim.github.io/posts/sklearn-for-TMVA-users/) help us a lot to set up a BDT using this library.

```python
    #For scikit-learn, one needs to concatenate all the data in one dataframe
    X_test_data =  df[:int(0.2*n_events)]#First 20% of dataset for testing BDT
    X_test_mc   =  df_mc[:int(0.2*n_events)]
    X_test      =  df.append(X_test_mc,ignore_index = True).values

    X_train_data =  df[int(0.2*n_events):n_events]#80% of dataset for training
    BDT
    X_train_mc   =  df_mc[int(0.2*n_events):n_events]
    X_train      =  X_train_data.append(X_train_mc,ignore_index = True).values

    # y_test and y_train are used to store categories (1 == signal, 0 ==
    background)
    y_test  = int(0.2*n_events)*[1]+int(0.2*n_events)*[0]
    y_train = int(0.8*n_events)*[1]+int(0.8*n_events)*[0]

    #DecisionTree settings
    dt = DecisionTreeClassifier(max_depth=3,
                                min_samples_leaf=0.05*len(X_train))

    #BoostedDecisionTree settings
    bdt = AdaBoostClassifier(dt, algorithm='SAMME',
                            n_estimators=800, learning_rate=0.5)
    #Training
    bdt.fit(X_train, y_train)
    
    #BDT output
    sk_y_predicted = bdt.predict(X_test) 
```
## Adding variable to the tree and saving output

Some analysis step could create a new variable, such as a BDT output, which is to be
added to a ROOT file. Two steps are needed to perform this :
- The variable should be added to the dataframe 
- The modified dataframe can be stored in a new ROOT file 

```python
    #Adding the BDT output to the dataframe
    df['BDT'] = sk_y_predicted

    #Storing the updated dataframe as a TTree in a ROOT file called output.root
    df.root_pandas.to_root(df, 'output.root', key='mytree')
```
## Going further

This note, plus the script and the jupyter notebook in the [repo](https://github.com/lhcb/impactkit-proceedings/tree/master/contrib-03-AnalysisWithPython), are
dedicated to help to setup a data analysis environment using python libraries.
A few simple analysis steps are showed but there is still a long road to build
a full analysis in this framework. Fortunately, there is already some great
material available such as :
- [scipy](https://www.scipy.org/index.html) for general scientific computations  
- [uncertainties](https://pythonhosted.org/uncertainties/), [mcerp](https://github.com/tisimst/mcerp) and [soerp](https://pypi.python.org/pypi/soerp) to deal with error propagation
- [iminuit](https://github.com/iminuit/iminuit) and [probfit](https://github.com/iminuit/probfit) for fitting

## Conclusion

During the ImpactKit, we were able to discover the python data analysis
environment and try by ourselves to build a small analysis workflow. It
demonstrates that in just a couple of hours, it is already possible to do some
basic analysis steps and create the basis of a MVA and store the result in
a ROOT file. 

If you want to try by yourself, you can load our jupyter notebook or have
a look at our scripts stored in our
[repo](https://github.com/lhcb/impactkit-proceedings/tree/master/contrib-03-AnalysisWithPython).
