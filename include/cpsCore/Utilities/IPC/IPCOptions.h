/*
 * PubOptions.h
 *
 *  Created on: Jul 15, 2019
 *      Author: mirco
 */

#ifndef UAVAP_CORE_IPC_IPCOPTIONS_H_
#define UAVAP_CORE_IPC_IPCOPTIONS_H_


struct IPCOptions
{
	bool multiTarget = true;
	bool retry = false;
	bool variableSize = false;
};



#endif /* UAVAP_CORE_IPC_IPCOPTIONS_H_ */
