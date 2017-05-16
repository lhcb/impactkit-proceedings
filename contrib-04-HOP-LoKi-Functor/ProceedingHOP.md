# Introduction

Carried out in the context of the 2016 Impactkit hackathon, this project aims to implement the ideas of the HOP mass, discussed in [LHCb-INT-2015-037](https://cds.cern.ch/record/2102345?ln=en), in the LHCb software to make them more accessible to all the collaboration members. This variable is conceived to exploit the kinematic characteristics of $B$ decays into final states involving electrons, by computing a mass in which the bremsstrahlung losses are balanced along the $B$ momentum. It is a variable of interest for all analyses of decays including at the same time electrons and other particles in the final state.

This note documents the algorithm used for the implementation of the HOP variables and also provides a pedagogical overview of how these tools have been implemented. In the first part, the algorithm chosen to build the HOP mass will be presented. The precise implementation of this variable as a LoKi functor is described in the second part while the third part focuses on the TupleTool implementation. 

# General algorithm to compute the HOP mass

The physics details are described in the first three sections of the note presented in the project description. As explained in the note we want to balance the three momentum components transversal to the $B$ flight direction of the electronic and hadronic parts of a given $B$ decay. The ratio of these transversal momenta defines the parameter $\alpha_{HOP}$:
$$\alpha_{\mathrm{HOP}} = \frac{P_t^{\mathrm{h}}}{P_t^{\mathrm{e}}}$$
this parameter is then used to scale the three-momentum of the electronic part of the decay:
$$\overrightarrow{P^{\mathrm{corr}}}(\mathrm{e^+ / e^-}) = \alpha_{\mathrm{HOP}} \times \overrightarrow{P^{\mathrm{corr}}}(\mathrm{e^+ / e^-})$$
This new three-momentum (together with the hadronic part) is later used to calculate the new invariant mass of the original $B$ particle. This accounts for the bremstrahlung losses of the final state electrons and positrons.

The algorithm will treat all the decays in the following way:

* find all the particles which further decay exclusively into electrons and positrons
	
* find all $e^{\pm}$ coming from particles that have both $e^\pm$ and other particles between their children
	
* add the four-momenta of the above-mentioned particles to obtain $P^{\mathrm{e}}$
	
* find all particles which have no $e^{\pm}$ in their decay tree or are non-electronic basic particles
	
* add the four-momenta of these particles to obtain $P^{\mathrm{h}}$
	
* calculate the three-momentum component $P_t^{\mathrm{e/h}}$ of $P^{\mathrm{e/h}}$ transversal to the original $B$ flight direction
	
* calculate $\alpha_{\mathrm{HOP}}$ as the ratio of these two quantities
	
* apply the $\alpha_{\mathrm{HOP}}$ correction as described above to all the $e^{\pm}$ in the final state 
	
* calculate the new corrected four-momenta of $e^{\pm}$ and sum them to obtain $P_{\mathrm{corr}}^{\mathrm{e}}$
	
* finally, add $P_{\mathrm{corr}}^{\mathrm{e}}$ and $P^{\mathrm{h}}$ and calculate the invariant mass of the resulting object

In this way the corrected HOP mass of the original $B$ is obtained.

# LoKi functor implementation

In order to implement the HOP mass as LoKi functors, the DaVinci software package has to be modified. The first step is to set up a development environment for the head version of `DaVinci` using the LHCb tools. The LoKi toolkit lives in the `Phys/LoKiPhys` package, the head version of which is download from the `Phys` repository. According to the usual LHCb convention, the C++ files are located in the `src` directory and header files are in the `LoKi` directory. The various LoKi functors are logically grouped in `Particles*.{cpp,h}` files.

The files in which the new HOP LoKi functors were implemented are `Particles38.{cpp,h}` because these files already contain the MCorrected (CORRM) and MCorrectedWithBestVertex (BPVCORRM) functors, which are to some extent similar to the HOP functors as they aim at correcting the mass of the parent particle using the missing momentum transverse to the direction of flight. MCorrectedWithBestVertex implicitly takes the best primary vertex of $B$, whereas the MCorrected uses the primary vertex that is provided as an input argument. Following this example, two new LoKi functors have been created: BremMCorrected (HOPM) and BremMCorrectedWithBestVertex (BPVCORRM). Additionaly, the computation of $p_{\mathrm{T}}$ with respect to the $B$ flight direction is already implemented in the LoKi code and it is used as it is.

The algorithm looks for all the children (of all generations) of the $B$ and creates three lists:

1. one containing all the non-electron children;

2. one containing all the particles that have only electrons and/or positrons as children;

3. a last one with the remaining electrons/positrons, namely those coming from particles that have both $e^\pm$ and other particles between their children.

The choice of the container in point 2. allows us to profit from the better mass resolution introduced by the PV refitting, where possible.

The four-momenta of all electrons and other particles are then summed up separately to obtain two `LorentzVector` variables `P_e_tot` and `P_h_tot` that are used to compute $\alpha$.
This is later used to correct the momenta of the electrons/positrons in the decay chain: it is multiplied by the space component of the four-momenta, while the energy component is re-computed as follows:
$$E_e = \sqrt{\alpha^2 * (p_X^ 2 + p_Y^ 2 +p_Z^ 2) + m_{e (PDG)}^2}.$$

The corrected four-momentum of the original particle is then computed summing the 4 momenta of the daughters after having applied this correction, and the invariant mass is returned.

The code and headers in `Particles38.{cpp,h}` also contain a version of the bremsstrahlung corrected mass that considers the best primary vertex: this just requires calling the previous method with this vertex as an input argument.

The final modification that is required to the LoKi code is to add the following lines in Phys/LoKiPhys/python/LoKiPhys/functions.py:
```python
# @see LoKi::Cuts::HOPM
HOPM    = LoKi.Particles.BremMCorrected
# @see LoKi::Cuts::BPVHOPM
BPVHOPM = LoKi.Particles.BremMCorrectedWithBestVertex ()
```
This informs DaVinci about the new LoKi Functors and gives them a name.

# TupleTool implementation

The TupleTool is also implemented in DaVinci but tuple tools live in the package `Phys/DecayTreeTuple` which is downloaded from the `Phys` project. The skeleton code for this TupleTool is based on a previously existing one: `TupleToolGeometry`. Since there was not a more similar one, a general TupleTool was chosen. It is recommended to start from the closest one to what you want to build.

All TupleTools should include at least: a constructor `TupleToolHOP::TupleToolHOP`, which inherits from `TupleToolBase`, an initializer `TupleToolHOP::initialize` where any external tool is loaded and initialized, and a fill method `TupleToolHOP::fill`, where the computations are done and the new variables are written to the ntuple. This last part is divided in three functions in order to simplify and reuse code:

- `TupleToolHOP::ClassifyParticles`: gets the top particle and two particle containers as input and loops over the decay chain classifying basic children in electron and non-electron type. When all the children of a composite particle belong to the same type, the composite particle receives the same type and it is itself -- instead of each children -- stored in the appropiate container. This is done in order not to loose presicion, since when the children are afterwards combined to obtain the mother's corrected mass no fit but a simple addition of momenta is used. 

- `TupleToolHOP::HOPProjectMomentum`: takes two particles as input, the first one is assumed to be the top of the decay. The direction of flight of this particle is obtained taking the best interaction vertex associated to it and its decay vertex. Then the transverse momentum of the second particle with respect to this direction is computed and returned.

- `TupleToolHOP::fill`: gets the top particle, the partice for which the variables should be computed, a string with the head name that will be used for the names of the variables that will be written to the ntuple and the ntuple itself as input. It first checks that the particle for which the variables should be computed is the same as the top of the chain. Since this algorithm is recursive on all the decay chain, it is only needed to run it once from the top of it. Then the daughters are classified using the `TupleToolHOP::ClassifyParticles` function and their transverse momentum with respect to the direction of flight of the top particle obtained with the `TupleToolHOP::HOPProjectMomentum`. The transverse momentum for all electron and non-electron particles are added up separately and the HOP ratio is computed. This value is stored in the ntuple in the variable head\_HOP. Afterwards, it is used to correct the 4-momentum of each particle in the electron container. Finally, all the 4-momenta of the electron particles are sumed together and added to the 4-momentum of the non-electron particles. The mother's corrected mass is obtained from this total corrected 4-momentum and stored in the ntuple with the name head\_HOP\_MASS. The electron corrected mass is also obtained from the corrected 4-momentum of all the electron particles added together and is saved as head\_HOP\_ELECTRON\_MASS.


# Results and comparision between tools

To check their performance, both tools are applied to \BdKstee and \BKemu simulated events and compared to the results from the ROOT macro used for the studies presented in in [LHCb-INT-2015-037](https://cds.cern.ch/record/2102345?ln=en). The HOP corrected mass is compared for both tools and the HOP ratio only for the TupleTool since this variable is also written to the output tree by the TupleTool but it's not returned by the LoKi functor.

In order to provide the user with a starting point to use these tools, selected lines from the script used to compare the TupleTool and the LoKi functor are shown in the following, without the configurations specific to the data set. The tests were performed using `DaVinci v40r1p3` along with the appropriate packages of the version `v11r7` of `Phys` for the tools. 

```
dtt = DecayTreeTuple('Bu2LLK_ee')
...
dtt.Decay = '[B0 -> K*(892)0 (J/psi(1S) -> e- e+ )]CC'
dtt.addBranches({'B0'    : '[^(B0 -> K*(892)0 (J/psi(1S) -> e- e+ ))]CC'})
hop_tool = dtt.B0.addTupleTool('TupleToolHOP')
b0_hybrid = dtt.B0.addTupleTool('LoKi::Hybrid::TupleTool/LoKi_B0')
b0_hybrid.Variables = {
    'hop_mass': 'BPVHOPM'
}
```

In this example, the decay tree is created first, as well as a branch for the `B0`. The TupleTool `TupleToolHOP` is then added to the branch `B0` using `addTupleTool`. To build leaves with LoKi functors, the `LoKi::Hybrid::TupleTool` has to be used and configured with the LoKi variable of interest, here `BPVHOPM`.

The results are presented in figures \ref{fig:B2Kstee-HOPMass}, \ref{fig:B2Kstee-HOP}, \ref{fig:B2Kemu-HOPMass} and \ref{fig:B2Kemu-HOP} showing a perfect agreement between both tools and the reference macro. It can be concluded then that the tools are working as expected and ready to be released.

![HOP corrected mass for simulated \BdKstee candidates\label{fig:B2Kstee-HOPMass}](figs/B2Kstee_HOPMass.png){ width=10cm }

![HOP ratio for simulated \BdKstee candidates\label{fig:B2Kstee-HOP}](figs/B2Kstee_HOP.png){ width=10cm }

![HOP corrected mass for simulated \BKemu candidates\label{fig:B2Kemu-HOPMass}](figs/B2Kemu_HOP.png){ width=10cm }

![HOP ratio for simulated \BKemu candidates\label{fig:B2Kemu-HOP}](figs/B2Kemu_HOPMass.png){ width=10cm }

# Conclusions

The concept of the HOP mass introduced in [LHCb-INT-2015-037](https://cds.cern.ch/record/2102345?ln=en) is implemented in the \lhcb software by means of a LoKi functor and a TupleTool. The technical details of the implementation and usage of the tools are described both to document them and also to serve as a guide on how to proceed when developing this kind of tools for the first time. Then the results are compared to those obtained for the studies in [LHCb-INT-2015-037](https://cds.cern.ch/record/2102345?ln=en). A perfect agreement is found, so the tools have been added to the \lhcb software. The LoKi functor and the TupleTool have been incorporated into the `Phys/LoKiPhys` and the `Phys/DecayTreeTuple` packages respectively. This allows all the analysts studying decays with electrons to take advantage of the HOP variables to reject and discriminate potential backgrounds.

# Acknowledgements

We want to express our immense gratitude to the organisers, teachers and helpers of the Impact Kit workshop for the nice and useful tutorials they prepared and shared with us. It was a very fruitful experience as proven in the work presented in this document. Special thanks go to Martino Borsato and Francesco Polci for being available and willing to answer all our questions about the HOP technique and providing their macros to ease the testing and debugging of the tools.
