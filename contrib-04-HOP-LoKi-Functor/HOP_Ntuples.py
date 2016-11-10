from Configurables import DecayTreeTuple
from DecayTreeTuple.Configuration import *
from LoKiPhys.decorators import BPVHOPM

# Stream and stripping line we want to use
stream = 'AllStreams'
line = 'Bu2LLK_eeLine2'

from GaudiConf import IOHelper

# Use the local input data
IOHelper().inputFiles([
    'root://eoslhcb.cern.ch//eos/lhcb/user/s/simone/RD/DST/MC11_Bd2KstEE.dst'
], clear=True)

from Configurables import DaVinci

# Configure DaVinci
DaVinci().InputType = 'DST'
DaVinci().TupleFile = 'DVntuple.root'
DaVinci().PrintFreq = 1000
DaVinci().DataType = '2011'
DaVinci().Simulation = True
# Only ask for luminosity information when not using simulated data
DaVinci().EvtMax = -1
DaVinci().CondDBtag = 'sim-20130522-vc-md100'
DaVinci().DDDBtag = 'dddb-20130929'
#MessageSvc().OutputLevel = DEBUG

# Create an ntuple to capture B decays from the StrippingLine line
dtt = DecayTreeTuple('Bu2LLK_ee')
dtt.Inputs = ['/Event/{0}/Phys/{1}/Particles'.format(stream, line)]
#dtt.Decay = '[B0 -> K*(892)0  e- e+]CC'
dtt.Decay = '[B0 -> ^K*(892)0 ^(J/psi(1S) -> ^e- ^e+ )]CC'

#dtt.addBranches({'B0'    : '[B0 -> K*(892)0 e- e+]CC',
#                 'Kstar' : '[B0 -> ^K*(892)0 e- e+]CC',
#                 'eplus' : '[B0 -> K*(892)0 ^e- e+]CC',
#                 'eminus': '[B0 -> K*(892)0 e- ^e+]CC'})
dtt.addBranches({'B0'    : '[^(B0 -> K*(892)0 (J/psi(1S) -> e- e+ ))]CC',
                 'Kst_892_0' : '[B0 -> ^K*(892)0 (J/psi(1S) -> e- e+ )]CC',
                 'E1' : '[B0 -> K*(892)0 (J/psi(1S) -> ^e- e+ )]CC',
                 'E2': '[B0 -> K*(892)0 (J/psi(1S) -> e- ^e+ )]CC',
                 'JPs': '[B0 -> K*(892)0 ^(J/psi(1S) -> e- e+ )]CC'})

b0_hybrid = dtt.B0.addTupleTool('LoKi::Hybrid::TupleTool/LoKi_B0')

b0_hybrid.Variables = {
    'hop_mass': 'BPVHOPM',
    'corr_mass': 'BPVCORRM'
}

DaVinci().UserAlgorithms += [dtt]
