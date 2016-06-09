# Analysis with python

## Motivation

In High Energy Physics, almost every data analysis framework involved the ROOT
software. Built in the 90's, it has been very useful to perform all the tasks
required by an analysis, from the handle of large datasets to the use of
powerful statistical procedures with the help of librairies such as RooFit or
TMVA.  
However, it hasn't spread outside of the HEP community while other projects
also dedicated to data analysis are now very mature with a much larger
users community.
The Goal of this project was to build an analysis demonstrator capable of :
- use the rich environment of python libraries to perform analysis tasks
- load and store processed data in ROOT file format, in order to be able to
  fully collaborate with ROOT users.

## Environment setup

In order to install and load all the packages we need for this project, we rely
on the [anaconda python distribution](https://www.continuum.io/downloads).

We tested the following environment on lxplus :

- Install miniconda (or anaconda) for python2.7, Linux 64 bit
  [link](http://conda.pydata.org/miniconda.html)
- A non-mandatory step would be to create a virtual environment to keep the
  local python distribution as clean as possible. It can be achieve trough
  : `conda create -n NameOfYourVirtualEnv python=2.7 anaconda`.
  To activate it, do `source activate NameOfYourVirtualEnv` and `source deactivate` to go back to normal. 
- Install ROOT inside the conda environment : `conda install -c chrisburr root=6.06.02`
- You will then need to activate the conda environment :
    - `source activate root` to load the main conda environment
    - `source activate NameOfYourVirtualEnv` if you made it yourself
- A few packages are also needed and can be installed using pip, the python
  package manager :
    - Numpy (arrays and tools for scientific computing), Matplotlib (plotting
      library), scikit-learn (Machine Learning in Python) : `pip install numpy matplotlib scikit-learn`
    - The [root_pandas package](https://github.com/ibab/root_pandas) is based on   [root_numpy](https://github.com/rootpy/root_numpy) and allows you to load a ROOT
      file and store the content in a pandas dataframe. Follow both links if
      you want more information on how to load a ROOT file. To install it, do
      `pip install root_pandas`
    - In order to produce a jupyter notebook which is a way to keep both the
      analysis code and its output in the same file, you can also do `pip install jupyter`. Then, to load a jupyter notebook, you can do
      `jupyter notebook` and access the notebook in your browser.

## Loading data

Using root_pandas library, we can load a ROOT file in a few simple steps :
- choose the file you want and store the path as string : `myfile = "path/to/file.root"`
- then select the branches you want by creating a list with the names of the
  variables you want. The use of of the wildcard **\*** is also supported. For
  instance you can select `mybranches = ["B0_M","*_PT"]` to load the B0 mass
  and the transverse momentum of each particle of your entry.
- As the file will be loaded in the memory, you should limit to a certain
  number of entries and set, for instance, `n_events = 10000`.
- Finally, select the name of your tuple as `tuple_name = 'someDir/myTree`

Then, you will be allowed to store you tree as a pandas dataframe using :
```python
   import root_pandas
   df = root_pandas.read_root(myfile, columns=mybranches, stop = n_events, key= tuple_name)
```
## Creating and plotting histograms

To produce a simple plot, let's say, the mass of the B0, you can simply do :
```python
   import numpy as np
   custom_binning = np.linspace(5000,5500,100) #min, max and number of bins 
   mass_B0 = df['B0_M'].plot.hist(bins=custom_binning, label = 'mass of the
   B^{0}_{M} [MeV/c$^{2}$]'} #label of the plot can be written in LaTeX
```

Then, you just need to add `plt.show()` in your code to plot the histogram.
A more detailed code is written in both `compare_mc_data.py` and
`AnalysisWithPython.ipynb`.  


## Cutting

One need to cut either to reduce a dataset or to produce some plot and there is
a way to both :
- create a reduce dataset : 
    ```python
    cut_string = " B0_M >5250 & B0_P > 500 "
    df_reduced = df.query(cut_string)
    ```   
- plot an histogram using a selection cut :
    ```python
        df.query(cut_string)['B0_M'].plot.hist(bins=custom_binning)
    ```

## MVA

ROOT users are used to TMVA when they need to train a BDT for their analysis.
The *scikit-learn* project is the most mature way to such task in Python.
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

Some analysis step could create a new variable, as a BDT output, worth to be
added to a ROOT file. Two steps are needed to perform this :
- The variable should be added to the dataframe using `df['BDT'] = sk_y_predicted`
- The modified dataframe can be stored in a new ROOT file using
  `df.root_pandas.to_root(df, 'output.root', key = 'mytree')`

## Conclusion

During the ImpactKit, we were able to discover the python data analysis
environment and try by ourselves to build a small analysis workflow. It
demonstrates that in just a couple of hours, it is already possible to do some
basic analysis steps and create the basis of a MVA and store the result in
a ROOT file. 

If you want to try by yourself, you can load our jupyter notebook or have
a look at our scripts stored in our
[repo](https://github.com/lhcb/impactkit-proceedings/tree/master/contrib-03-AnalysisWithPython).
