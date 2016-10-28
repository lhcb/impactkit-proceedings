
# Introduction

The goal of this project is to create a new LoKi functor and a new TupleTool to implement the ideas of the HOP mass, discussed in [LHCb-INT-2015-037](https://cds.cern.ch/record/2102345?ln=en). This tool is conceived to exploit the kinematic characteristics of $B$ decays into final states involving electrons, by computing a mass in which the bremsstrahlung losses are balanced along the $B$ momentum. The computation of $p_{\mathrm{T}}$ with respect to the $B$ flight direction is already implemented in the LoKi code. This is used later in the LoKi functor implementation of the HOP mass. However, this is not the case for the TupleTool, so this function is implemented as well in the new tool.

# Physics details

The physics details are described in the first three sections of the note presented in the project description. As explained in the note we want to balance the three momentum components transversal to the $B$ flight direction of the electronic and hadronic parts of a given $B$ decay. The ratio of these transversal momenta defines the parameter $\alpha_{HOP}$:
$$\alpha_{\mathrm{HOP}} = \frac{P_t^{\mathrm{h}}}{P_t^{\mathrm{e}}}$$
this parameter is then used to scale the three-momentum of the electronic part of the decay:
$$\overrightarrow{P^{\mathrm{corr}}}(\mathrm{e^+ / e^-}) = \alpha_{\mathrm{HOP}} \times \overrightarrow{P^{\mathrm{corr}}}(\mathrm{e^+ / e^-})$$
This new three-momentum (together with the hadronic part) is later used to calculate the new invariant mass of the original $B$ particle. This accounts for the bremstrahlung losses of the final state electrons and positrons.

Our algorithm will treat all the decays in the following way:

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

This way we are left with the corrected HOP mass of the original $B$.

# LoKi functor implementation

In order to solve this task we have to make changes to the DaVinci software package. So first, we will need to set up a development environment. Use the `lb-dev` command:

```bash
lb-dev --name DaVinciDevWithHOPLoKi DaVinci v39r1p1
```

This creates `DaVinciDevWithHOPLoKi` folder, which will be our working folder. 

The LoKi toolkit lives in the `Phys/LoKiPhys` package. So first we move into the `DaVinciDevWithHOPLoKi` folder and download the package from the repository.

```bash
cd ./DaVinciDevWithHOPLoKi
getpack Phys/LoKiPhys
```

Now you will be asked about the version of package you want to download. At the time of writing of this tutorial the reliably working version was `v11r7` so choose that one. This creates a new `Phys/LoKiPhys` containing the LoKiPhys code. According to the usual LHCb convention the C++ files are located in the `src` directory and header files are in the `LoKi` directory. The various LoKi functors are logically grouped in `Particles*.{cpp,h}` files. The files where we implemented our new HOP LoKi functor is `Particles38.{cpp,h}`, since these files already contain the MCorrected (CORRM) and MCorrectedWithBestVertex (BPVCORRM) functors, which will serve as a skeleton for our new functor. MCorrectedWithBestVertex implicitly takes the best primary vertex of $B$, whereas the MCorrected uses the primary vertex that is provided as an input argument. Following this example, we also create two new LoKi functors: BremMCorrected (HOPM) and BremMCorrectedWithBestVertex (BPVCORRM).

The algorithm we developed looks for all the children (of all generations) of the $B$ and creates three lists:

1. one containing all the non-electron children;

2. one containing all the particles that have only electrons and/or positrons as children;

3. a last one with the remaining electrons/positrons, namely those coming from particles that have both $e^\pm$ and other particles between their children.

The choice of the container in point 2. allows us to profit from the better mass resolution introduced by the PV refitting, where possible.

The four-momenta of all electrons and other particles are then summed up separately to obtain two `LorentzVector` variables `P_e_tot` and `P_h_tot` that are used to compute $\alpha$.
This is later used to correct the momenta of the electrons/positrons in the decay chain: it is multiplied by the space component of the four-momenta, while the energy component is re-computed as follows:
$$E_e = \sqrt{\alpha^2 * (p_X^ 2 + p_Y^ 2 +p_Z^ 2) + m_{e (PDG)}^2}$$

The corrected four-momentum of the original particle is then computed summing the 4 momenta of the daughters after having applied this correction, and the invariant mass is returned.

The code and headers in `Particles38.{cpp,h}` are repeated a second time for implementing the version of the code that considers the user-defined primary vertex: this just requires adding this vertex as an input argument.

The final modification that is required to the LoKi code is to add the following lines in Phys/LoKiPhys/python/LoKiPhys/functions.py:
```python
# @see LoKi::Cuts::HOPM
HOPM    = LoKi.Particles.BremMCorrected
# @see LoKi::Cuts::BPVHOPM
BPVHOPM = LoKi.Particles.BremMCorrectedWithBestVertex ()  
```
This informs DaVinci about the new LoKi Functors and gives them a name.

# TupleTool implementation

The TupleTool is also implemented in DaVinci. In this case, we started from the most recent version available when the Impactkit took place, v40r1p3. All tuple tools live in the package `Phys/DecayTreeTuple`, so we need to download it:

```bash
lb-dev --name DaVinciDevWithHOPTupleTool DaVinci v40r1p3
cd ./DaVinciDevWithHOPTupleTool
getpack Phys/DecayTreeTuple
```

Similarly to the LoKi functor case, the scheleton code for this TupleTool is based in a previous existing one: `TupleToolGeometry`. Since there was not a more similar one, we chose a general TupleTool. We recommend to start from the closest one to what you want to build.

All TupleTools should include at least: a constructor `TupleToolHOP::TupleToolHOP`, which inherits from `TupleToolBase`, an initializer `TupleToolHOP::initialize` where any external tool is loaded and initialized, and a fill method `TupleToolHOP::fill`, where the computations are done and the new variables are written to the ntuple. We choose to divide this last part in three functions in order to simplify and reuse code and we strongly suggest to do so:

- `TupleToolHOP::ClassifyParticles`: gets the top particle and two particle containers as input and loops over the decay chain classifying basic children in electron and non-electron type. When all the children of a composite particle belong to the same type, the composite particle receives the same type and it is itself -- instead of each children -- stored in the appropiate container. This is done in order not to loose presicion, since when the children are afterwards combined to obtain the mother's corrected mass no fit but a simple addition of momenta is used. 

- `TupleToolHOP::HOPProjectMomentum`: takes two particles as input, the first one is assumed to be the top of the decay. The direction of flight of this particle is obtained taking the best interaction vertex associated to it and its decay vertex. Then the transverse momentum of the second particle with respect to this direction is computed and returned.

- `TupleToolHOP::fill`: gets the top particle, the partice for which the variables should be computed, a string with the head name that will be used for the names of the variables that will be written to the ntuple and the ntuple itself as input. It first checks that the particle for which the variables should be computed is the same as the top of the chain. Since our algorithm is recursive on all the decay chain we only need to run it once from the top of it. Then the daughters are classified using the `TupleToolHOP::ClassifyParticles` function and their transverse momentum with respect to the direction of flight of the top particle obtained with the `TupleToolHOP::HOPProjectMomentum`. The transverse momentum for all electron and non-electron particles are added up separately and the HOP ratio is computed. This value is stored in the ntuple in the variable head\_HOP. Afterwards, it is used to correct the 4-momentum of each particle in the electron container. Finally, all the 4-momenta of the electron particles are sumed together and added to the 4-momentum of the non-electron particles. The mother's corrected mass is obtained from this total corrected 4-momentum and stored in the ntuple with the name head\_HOP\_MASS. The electron corrected mass is also obtained from the corrected 4-momentum of all the electron particles added together and is saved as head\_HOP\_ELECTRON\_MASS.


# Results and comparision between tools

To check their performance both tools are applied to \BdKstee and \BKemu simulated events and compared to the results from the ROOT macro used for the studies presented in in [LHCb-INT-2015-037](https://cds.cern.ch/record/2102345?ln=en). The HOP corrected mass is compared for both tools and the HOP ratio only for the TupleTool since this variable is also written to the output tree by the TupleTool but it's not returned by the LoKi functor. Results are presented in figures \ref{fig:B2Kstee-HOPMass}, \ref{fig:B2Kstee-HOP}, \ref{fig:B2Kemu-HOPMass} and \ref{fig:B2Kemu-HOP} showing a perfect agreement between both tools and the reference macro. It can be concluded then that the tools are working as expected and ready to be released.

![HOP corrected mass for simulated \BdKstee candidates\label{fig:B2Kstee-HOPMass}](figs/B2Kstee_HOPMass.png){ width=10cm }

![HOP ratio for simulated \BdKstee candidates\label{fig:B2Kstee-HOP}](figs/B2Kstee_HOP.png){ width=10cm }

![HOP corrected mass for simulated \BKemu candidates\label{fig:B2Kemu-HOPMass}](figs/B2Kemu_HOP.png){ width=10cm }

![HOP ratio for simulated \BKemu candidates\label{fig:B2Kemu-HOP}](figs/B2Kemu_HOPMass.png){ width=10cm }

# Conclusions

The concept of the HOP mass introduced in [LHCb-INT-2015-037](https://cds.cern.ch/record/2102345?ln=en) is implemented in the \lhcb software by means of a LoKi functor and a TupleTool. The technical details of the implementation and usage of the tools are described both to document them and also to serve as a guide on how to proceed when developing this kind of tools for the first time. Then the results are compared to those obtained for the studies in [LHCb-INT-2015-037](https://cds.cern.ch/record/2102345?ln=en). A perfect agreement is found proving that the tools are working as expected and ready to be incorporated to the \lhcb software.