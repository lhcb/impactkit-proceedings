
# New LoKi functor: HOP mass

## Description of the project

Create a new LoKi functor to implement the ideas of the HOP mass, discussed in https://cds.cern.ch/record/2102345?ln=en. In particular, the team needs to check if the $p_{\mathrm{T}}$ with respect to the $B$ flight direction is implemented, and if not, do so. Afterwards, implement HOP and the validation variables discussed in the note ($\theta(ee)$ and $\theta(hadrons)$). This shares the physics content with the TupleTool, so collaboration between the two teams is foreseen.

## Solution to the problem

### Setting up the environment

In order to solve this task we have to make changes to the DaVinci software package. So first, we will need to set up a development environment. Use the `lb-dev` command:

`lb-dev --name DaVinciDevWithHOPLoKi DaVinci v39r1p1`

This creates `DaVinciDevWithHOPLoKi` folder, which will be our working folder. 

The LoKi toolkit lives in the `Phys/LoKiPhys` package. So first we move into the `DaVinciDevWithHOPLoKi` folder and download the package from the repository.

```bash
mv ./DaVinciDevWithHOPLoKi
getpack Phys/LoKiPhys
```

Now you will be asked about the version of package you want to download. At the time of writing of this tutorial the reliably working version was `v11r7` so choose that one. After pressing enter you should have a new `Phys/LoKiPhys` directory with a lot of subdirectories and files in your working folder. According to the usual LHCb convention the C++ files are located in the `src` directory and header files are in the `LoKi` directory. The various LoKi functors are logically grouped in `Particles*.{cpp,h}` files. The files where we put our new HOP LoKi functor is `Particles38.{cpp,h}`. These files were chosen because they already contain `CORRM` functor, which will serve as a skeleton for our new functor.

### The physics

Now is the correct time to think about the physics behind the problem. For this we recommend to read at least the first three sections of the note presented in the project description. As explained in the note we want to balance the three momentum components transversal to the $B$ flight direction of electronic and hadronic parts of a given $B$ decay. The ratio of these transversal momenta defines the parameter $\alpha_{HOP}$:
$$\alpha_{HOP} = \frac{P_t^{hadron}}{P_t^{electron}}$$
this parameter is then used to scale the three-momentum of the electronic part of the decay:
$$\overrightarrow{P^{\mathrm{corr}}}(\mathrm{e^+ / e^-}) = \alpha_{HOP} \times \overrightarrow{P^{\mathrm{corr}}}(\mathrm{e^+ / e^-})$$
this new three-momentum (together with the hadronic part) is then used to calculate the new invariant mass of the original particle. In theory this supposed to account for the bremstrallung losses of the final state electrons and positrons.

### The LoKi functor


