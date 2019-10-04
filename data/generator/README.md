The following directory contains data files that can be used by restG4 as an external generator. Decay0 files include event energies and momentum distributions for the different particle types produced in each event.

They should be included in the `generator` definition in the `TRestG4Metadata` section as follows

```
<generator type="volume" from="gasVolume" >
    <source fromFile="Xe136nldbdM1.dat" > </source>
</generator>
```

The following describes the contents of the data files inside the directory, contributed by different authors.

- Author : Gloria Luzon
- Date : 22-Feb-2016
- Description : 
- Generated using Decay0 :  arxiv.org/pdf/nucl-ex/0104018v1.pdf
---------------------

 	- BiPo212.dat --> Bi212(beta)+ Po112 (alpha) decays
	- BiPo214.dat --> Bi214(beta)+ Po114 (alpha) decays
 	- K40.dat   --> K40 decays
 	- Pb212.dat--> Pb212 beta decay to Bi212
 	- Pb214.dat--> Pb214 beta decay to Bi214
	- Tl208.dat--> Tl208 beta decay to Pb208

	- Xe136bb0n.dat
		event type: Xe136           
              0nubb(mn) 0+ -> 0+     {2n}      
              level, Elevel (MeV) =  0+       0.0000     MeV

	- Xe136bb2n.dat ::: REMOVED FILE leading to wrong results 29/Sep/2019. Needs regeneration and validation.
		event type: Xe136           
              2nubb     0+ -> 0+     {2n}      
              level, Elevel (MeV) =  0+       0.0000     MeV
              

- Author: Xie Chen
- Date: 2019-4-23
- Description : The new updated .dat:the decay of Xe136 to excited state of Ba136 
- Generated using Decay0 : https://github.com/BxCppDev/bxdecay0.
---------------------

    - Filename                         nevent          process                                                 decay mode
    - Xe136nldbdM1.dat:        nevent=20000     Xe136 0+->Ba 0+                        bxdecay0::MODEBB_1 : 0nubb(mn), with neutrino mass, 0+ -> 0+ {2n}

    - Xe136bb0n21PlusM7.dat    nevent=20000    Xe136 0+->Ba136 21+ with 1 gamma        bxdecay0::MODEBB_7 : 0nubb(rhc-lambda), with rhc-lambda, 0+ -> 2+ {2n}

    - Xe136bb0n22PlusM7.dat    nevent=20000    Xe136 0+->Ba136 22+ with 1 or 2 gamma{  bxdecay0::MODEBB_7 : 0nubb(rhc-lambda), with rhc-lambda, 0+ -> 2+ {2n}
                branch ratio:47.1%  22+->21+(732.455keV)->0+(818.515keV) 2gamma
                branch ratio:52.9%  22+->0+(1550.97keV)                  1gamma}

    - Xe136bb0n01PlusM1.dat    nevent=20000    Xe136 0+->Ba136 01+ with 2 gamma        bxdecay0::MODEBB_1 : 0nubb(mn), with neutrino mass, 0+ -> 0+ {2n}
                (M1,M7 mean the decay mode of MODEBB_1 and MODEBB_7)
