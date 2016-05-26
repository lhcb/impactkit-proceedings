import root_pandas
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

from sklearn import datasets
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import AdaBoostClassifier
from sklearn.metrics import classification_report, roc_auc_score
from sklearn.cross_validation import train_test_split

# DEFINE SOME NAMES AND PATHS
file_path  ='root://eoslhcb.cern.ch//eos/lhcb/user/a/apearce/Impactkit/AnalysisWithPython/'
n_events   = 100000
var_list   = ['Dp_M','Dp_Y','Dp_IPCHI2_OWNPV','Dp_FDCHI2_OWNPV','*_PT','Dp_PT_First', 'Dp_PT_Second', 'Dp_PT_Third', 'Dp_h1_TRACK_CHI2NDOF', 'Dp_h2_TRACK_CHI2NDOF', 'Dp_h3_TRACK_CHI2NDOF', 'Dp_Loki_MIPCHI2DV_First', 'Dp_Loki_MIPCHI2DV_Second', 'Dp_Loki_MIPCHI2DV_Third', 'Dp_h2_PIDK', 'Dp_h3_PIDK', 'Dp_h1_PIDK', 'Dp_ENDVERTEX_CHI2','Dp_ENDVERTEX_NDOF', 'Dp_DIRA_OWNPV', 'Dp_FDCHI2_OWNPV', 'Dp_Loki_BPVLTIME']
hlt2_cut_string = 'Dp_PT_First > 1000.0 & Dp_PT_Second > 400.0 & Dp_PT_Third > 200.0 & Dp_h1_TRACK_CHI2NDOF < 3.0 & Dp_h2_TRACK_CHI2NDOF < 3.0 & Dp_h3_TRACK_CHI2NDOF < 3.0 & Dp_Loki_MIPCHI2DV_First > 50.0 & Dp_Loki_MIPCHI2DV_Second > 10.0 & Dp_Loki_MIPCHI2DV_Third > 4.0 & Dp_h2_PIDK < 5.0 & Dp_h3_PIDK < 5.0 & Dp_h1_PIDK > 5.0 & Dp_M > 1789.0 & Dp_M < 1949.0 & Dp_ENDVERTEX_CHI2/Dp_ENDVERTEX_NDOF < 25.0 & Dp_DIRA_OWNPV > 0.9994 & Dp_FDCHI2_OWNPV > 16.0 & Dp_Loki_BPVLTIME > 0.00015'
tuple_name = 'TupleDpToKpipi/DecayTree'

print "LOADING DATA"
df       = root_pandas.read_root(file_path+'DVntuple_Real.root',  columns=var_list, stop = 5*n_events, key=          tuple_name)
df_cheat = root_pandas.read_root(file_path+'DVntuple_Cheat.root', columns=var_list, stop = 4*n_events,   key='Cheated'+tuple_name)
print "DATA IS LOADED"

vars_to_plot = df.columns
binning      = {'Dp_M' :np.arange(1600,2004,4),
                'Dp_PT':np.arange(0,1.5e4+1e3,1e3),
		'Dp_Y' :np.arange(2,4.55,0.25), 
		'Dp_h1_PT':np.arange(0,1e4+5e1,5e1),
     	        'Dp_h2_PT':np.arange(0,1e4+5e1,5e1),
     	        'Dp_h3_PT':np.arange(0,1e4+5e1,5e1),
	        'Dp_IPCHI2_OWNPV':np.arange(-1e1,1e1+1e-1,1e-1),
		'Dp_FDCHI2_OWNPV':np.arange(-1e1,1e1+1e-1,1e-1), 
	       }

# PLOT SOME OF THE VARIABLES 
for var in vars_to_plot:
    if var == 'Dp_PT_First':
        break
    elif 'OWNPV' not in var:
	data_hist = df.query('Dp_M >1855 & Dp_M < 1885')[var].plot.hist(         bins=binning[var],log=True,alpha=0.5,label = '{0} Data'.format(var),normed=True)
	mc_hist   = df_cheat.query('Dp_M >1855 & Dp_M < 1885 & '+hlt2_cut_string)[var].plot.hist(              bins=binning[var],log=True,alpha=0.5,label = '{0}   MC'.format(var),normed=True)
    else:
        data_hist = np.log(df.query('Dp_M >1855 & Dp_M < 1885'))[var].plot.hist( bins=binning[var],log=True,alpha=0.5,label = '{0} Data'.format(var),normed=True)
	mc_hist   = np.log(df_cheat.query('Dp_M >1855 & Dp_M < 1885 & '+hlt2_cut_string)[var]).plot.hist(      bins=binning[var],log=True,alpha=0.5,label = '{0}   MC'.format(var),normed=True)

    # ADD LABELS
    data_patch = mpatches.Patch(color='blue' , label='{0} data'.format(var))
    mc_patch   = mpatches.Patch(color='green', label='{0}   MC'.format(var))

    plt.legend(handles=[data_patch,mc_patch],loc = 'upper left')
    plt.ylabel('Frequency')
    plt.xlabel(var)
    plt.title(r"'$D^+ \rightarrow K^- \pi^+ \pi^+$ Data vs. MC'")
 
    plot_name = 'SelectionPlots/DptoKpipi_{0}_DataVsMC.pdf'.format(var)
    plt.savefig(plot_name)
    print "Figure was saved in "+plot_name
    plt.close()

# 2D PLOT
kin_hist = plt.hist2d(df['Dp_PT'], df['Dp_Y'], bins=[binning['Dp_PT'],binning['Dp_Y']])
plt.colorbar()
plt.savefig('SelectionPlots/DptoKpipi_PT_Y_Data.pdf')
plt.clf()

# RUN BDT TRAINING
features = ['Dp_IPCHI2_OWNPV','Dp_FDCHI2_OWNPV','Dp_h1_PT']


# TEST AND TRAINING SAMPLES CONTAIN SIGNAL AND BACKGROUND
X_test_data =  df.query('Dp_M<1850 | Dp_M>1890')[features][:int(0.2*n_events)]
X_test_mc   =  df_cheat.query(hlt2_cut_string)[features][:int(0.2*n_events)]
X_test      =  X_test_data.append(X_test_mc,ignore_index = True).values

X_train_data =  df.query('Dp_M<1850 | Dp_M>1890')[features][int(0.2*n_events):n_events]
X_train_mc   =  df_cheat.query(hlt2_cut_string)[features][int(0.2*n_events):n_events]
X_train      =  X_train_data.append(X_train_mc,ignore_index = True).values

y_test  = int(0.2*n_events)*[1]+int(0.2*n_events)*[0]
y_train = int(0.8*n_events)*[1]+int(0.8*n_events)*[0]

dt = DecisionTreeClassifier(max_depth=3,
                            min_samples_leaf=0.05*len(X_train))
bdt = AdaBoostClassifier(dt,
			 algorithm='SAMME',
			 n_estimators=800,
			 learning_rate=0.5)

bdt.fit(X_train, y_train)
sk_y_predicted = bdt.predict(X_test)
print classification_report(y_test, sk_y_predicted, target_names=["background", "signal"])
print "Area under ROC curve: %.4f"%(roc_auc_score(y_test, sk_y_predicted))

plt.hist(bdt.decision_function(X_test_data).ravel(),color='r', alpha=0.5, range=(-0.4,0.4), bins=30)
plt.hist(bdt.decision_function(X_test_mc).ravel()  ,color='b', alpha=0.5, range=(-0.4,0.4), bins=30)
plt.xlabel("scikit-learn BDT output")

plt.savefig('BDT.pdf')

