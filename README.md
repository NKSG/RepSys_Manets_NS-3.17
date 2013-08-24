RepSys_Manets_NS-3.17
=============

Developer	: Deepak Ananth Rama; Nagendra Akula Suresh Babu; Zongsheng Li
email		  : deepak.ananthrama@gmail.com
Date		  : December 2012
For		    : Research paper titled "Reputation System for Wireless Networks" based on the work on the same topic by Chuchu Wu towards her PhD thesis

Abstract
  No matter in a social network or a wireless network, co-operation among nodes is not only beneficial, but also essential. Mobile Ad hoc Networks or MANETs for short are multihop networks where each node also plays the role of a forwarder for other nodes packets. These intermediate nodes play a vital role and form the backbone of the network. Network throughput which is affected by various channel factors can be significantly improved by applying corrective measures such as network coding during a packet forward. However these actions imply extra resource usage at these nodes. Selfish forwarders may simply forward packets without any network coding or may drop some or all of the packets which are not intended for it. This affects the throughput and security in the network. At one extreme if none of these nodes help each other, the network operation itself ceases. To encourage all nodes to be cooperative, this project implements a concept of Social Norm based incentive scheme based on a Reputation System. Nodes are punished for dropping packets and rewarded for forwarding packets by broadcasting its reputation among neighbors. The social game continues with each node basing its decision to forward/drop packets based on the reputation of the end to end nodes of the packet. We evaluate this design via simulation on the NS-3 and and analyze performance of the system under various parameter settings of number of nodes, mobility, throughput and packet loss.



To get started with ns-3, please consult the project tutorial available
at http://www.nsnam.org/documentation.

Clone this repository

cd to the NS root folder "ns-3.17"
  Run "./waf configure -d debug --enable-examples --enable-tests" to configure
  Run "./waf" to run a re-build
  Run "./test.py -c core" to test 
  Run "./waf --run hello-simulator" to run the script "hello-simulator"
  

