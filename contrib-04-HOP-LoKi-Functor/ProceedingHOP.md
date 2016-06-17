
# Introducing the HOP mass in the LHCb software


## Introduction

The goal of this project is to create a new LoKi functor and a new TupleTool to implement the ideas of the HOP mass, discussed in https://cds.cern.ch/record/2102345?ln=en. This tool is conceived to exploit the kinematic characteristics of $B$ decays into final states involving electrons, by computing a mass in which the bremsstrahlung losses are balanced along the $B$ momentum. The $p_{\mathrm{T}}$ with respect to the $B$ flight direction is already implemented in the LoKi code. We use this latter in the HOP tool.

## Physics details

For this we recommend to read at least the first three sections of the note presented in the project description. As explained     in the note we want to balance the three momentum components transversal to the $B$ flight direction of electronic and hadron    ic parts of a given $B$ decay. The ratio of these transversal momenta defines the parameter $\alpha_{HOP}$:
$$\alpha_{\mathrm{HOP}} = \frac{P_t^{\mathrm{h}}}{P_t^{\mathrm{e}}}$$
this parameter is then used to scale the three-momentum of the electronic part of the decay:
$$\overrightarrow{P^{\mathrm{corr}}}(\mathrm{e^+ / e^-}) = \alpha_{\mathrm{HOP}} \times \overrightarrow{P^{\mathrm{corr}}}(\ma    thrm{e^+ / e^-})$$
this new three-momentum (together with the hadronic part) is then used to calculate the new invariant mass of the original par    ticle. In theory this supposed to account for the bremstrallung losses of the final state electrons and positrons.

Our algorithm will treat all the decays in the following way:

* find all the particles which further decay only into $e^{\pm}$
* find all $e^{\pm}$ coming from semi-leptonic decays
* add the four-momenta of these particles to obtain $P^{\mathrm{e}}$
* find all particles which have no $e^{\pm}$ in their decay tree or are non-electronic basic particles
* add the four-momenta of these particles to obtain $P^{\mathrm{h}}$
* calculate the three-momentum component $P_t^{\mathrm{e/h}}$ of $P^{\mathrm{e/h}}$ transversal to the original $B$ flight dir    ection
* calculate $\alpha_{\mathrm{HOP}}$ as the ratio of these two quantities
* apply the $\alpha_{\mathrm{HOP}}$ correction as described above to all the $e^{\pm}$ in the final state
* calculate the new corrected four-momenta of $e^{\pm}$ and sum them to obtain $P_{\mathrm{corr}}^{\mathrm{e}}$
* finally, add $P_{\mathrm{corr}}^{\mathrm{e}}$ and $P^{\mathrm{h}}$ and calculate the invariant mass of the resulting object

This way we should be left with the corrected HOP mass of the original $B$.

## LoKi functor implementation

In order to solve this task we have to make changes to the DaVinci software package. So first, we will need to set up a development environment. Use the `lb-dev` command:

`lb-dev --name DaVinciDevWithHOPLoKi DaVinci v39r1p1`

This creates `DaVinciDevWithHOPLoKi` folder, which will be our working folder. 

The LoKi toolkit lives in the `Phys/LoKiPhys` package. So first we move into the `DaVinciDevWithHOPLoKi` folder and download the package from the repository.

```bash
mv ./DaVinciDevWithHOPLoKi
getpack Phys/LoKiPhys
```

Now you will be asked about the version of package you want to download. At the time of writing of this tutorial the reliably working version was `v11r7` so choose that one. This creates a new `Phys/LoKiPhys` containing the LoKiPhys code. According to the usual LHCb convention the C++ files are located in the `src` directory and header files are in the `LoKi` directory. The various LoKi functors are logically grouped in `Particles*.{cpp,h}` files. The files where we implemented our new HOP LoKi functor is `Particles38.{cpp,h}`, since these files already contain the MCorrected (CORRM) and MCorrectedWithBestVertex (BPVCORRM) functors, which will serve as a skeleton for our new functor. MCorrectedWithBestVertex implicitly takes the best primary vertex of $B$, whereas the MCorrected takes the primary vertex of $B$ as an input argument. Following this example, we also create two new LoKi functors: BremMCorrected (HOPM) and BremMCorrectedWithBestVertex (BPVCORRM).

The first algorithm we developed looks for all the children (of all generations) of $B$ and creates three lists:

1. one containing all the non-electron children;
2. one containing all the particles that have only electrons and/or positrons as childrens;
3. a last one with the remaining electrons/positrons, namely those coming from particles that have both $e^\pm$ and other particles between their childres.

This strategy allows us to profit from the better mass resolution introduced by the PV refitting, where possible (point 2.).

We implemented at this purpose the `e_finder` and the `has_electron` functions.

The four-momenta of all electrons and other particles are then summed up separately to obtain two `LorentzVector` variables `P_e_tot` and `P_h_tot` that are used to compute $\alpha$.
This latter is then used to correct the momenta of the electronic part of the decay chain: it is multiplied to the space component of the four-momenta, while the energy component is re-computed as follows:
$$E_e = \sqrt{\alpha^2 * (p_X^ 2 + p_Y^ 2 +p_Z^ 2) + m_{e (PDG)}^2}$$

The mother's corrected four-momentum is then computed summing the 4 moemnta of the daughters after having applied this correction, and the invariant mass is returned.

## TupleTool implementation

## Results and comparision between tools

## Conclusions
