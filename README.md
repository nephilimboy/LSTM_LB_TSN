# An adaptive LSTM-based load balancer for TSN-cloud integration
With ongoing advancements in the industry, integrating Time-Sensitive Networking (TSN) standards and cloud computing has become crucial 
to enabling reliable, low-latency communication between factory industrial machines and their controller software hosted in edge clouds. 
This integration begins by implementing standard TSN functions as executable code, which can be deployed as Virtual Network Functions 
(VNFs) or as containers alongside the industrial controller software in the cloud. Additionally, due to the dynamic nature of cloud 
environments and the likelihood of application or service failures, deploying multiple replicas of controller software instances with a 
load balancer is common practice. However, current cloud-based load balancers are not fully compatible with TSN standards, 
lacking the capability for effective flow management of hyper-reliable and time-sensitive traffic and adaptation to instance failures. 
To that end, this Project proposes a load balancer architecture that is compatible with TSN standards. The architecture incorporates a 
neural network-based failure detection mechanism, enabling quick adjustment of its internal round-robin algorithm when application 
instance replica fails.

## Requirements 
* Kubernetes >= 1.28
* Flannel CNI
* OvS CNI
* Multus CNI
* Python >= 3.7
* nload (https://github.com/rolandriegel/nload)

## Installation Steps
* Build load balancer
  * ``Make; Make install``
* Run nload
* Run LSTM model (Failure detection)
  * Python3.10 Model.py
