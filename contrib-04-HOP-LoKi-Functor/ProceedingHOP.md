
# Introducing the HOP mass in the LHCb software


## Introduction

The goal of this project is to create a new LoKi functor and a new TupleTool to implement the ideas of the HOP mass, discussed in https://cds.cern.ch/record/2102345?ln=en. This tool is conceived to exploit the kinematic characteristics of $B$ decays into final states involving electrons, by computing a mass in which the bremsstrahlung losses are balanced along the $B$ momentum. The $p_{\mathrm{T}}$ with respect to the $B$ flight direction is already implemented in the LoKi code. We use this latter in the HOP tool.

## Physics details

For this we recommend to read at least the first three sections of the note presented in the project description. As explained in the note we want to balance the three momentum components transversal to the $B$ flight direction of electronic and hadronic parts of a given $B$ decay. The ratio of these transversal momenta defines the parameter $\alpha_{HOP}$:
![equation](http://www.sciweavers.org/tex2img.php?eq=%5Calpha_%7BHOP%7D%20%3D%20%5Cfrac%7BP_t%5E%7Bhadron%7D%7D%7BP_t%5E%7Belectron%7D%7D&bc=White&fc=Black&im=jpg&fs=12&ff=arev&edit=0)

$$\alpha_{HOP} = \frac{P_t^{hadron}}{P_t^{electron}}$$
this parameter is then used to scale the three-momentum of the electronic part of the decay:
$$\overrightarrow{P^{\mathrm{corr}}}(\mathrm{e^+ / e^-}) = \alpha_{HOP} \times \overrightarrow{P^{\mathrm{corr}}}(\mathrm{e^+ / e^-})$$
this new three-momentum (together with the hadronic part) is then used to calculate the new invariant mass of the original particle. In theory this supposed to account for the bremstrallung losses of the final state electrons and positrons.

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
