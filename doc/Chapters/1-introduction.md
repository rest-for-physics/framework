## Introduction

REST is an event-oriented analysis framework. Based on ROOT, it defines specific classes to keep data,
read config, and run processes. Most of these classes are designed for gas detectors with micropatterned 
readout. 

REST does analysis through some modular process classes. They in chain convertes raw data to the final 
analysis result. One can easily modify the process chain in the config file and do his own analysis.
We will frequently introduce new or update the old processes as the analysis work requires. 
The user can also write his own processes at any time.

History of developments:

>2014-Jun: First concept. As part of conceptualization of previous REST code (REST v2)  
         Igor G. Irastorza  


>2017-Aug: Major change to xml reading and program running strategy  
         Kaixiang Ni  


[**prev**](0-contents.md)
[**contents**](0-contents.md)
[**next**](2-installing-rest.md)