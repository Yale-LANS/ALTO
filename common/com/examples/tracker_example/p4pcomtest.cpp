/*
 * P4P COM library example (Win32)
 */

#include "stdafx.h"
#include <p4pcom/p4pcom.h>

int _tmain(int argc, _TCHAR* argv[])
{
	/* Get the (singleton) P4P factory */
	IP4PFactory *pP4P = NULL;
	GetIP4PFactory(&pP4P);
	
	/* Set log level */
	pP4P->SetLogLevel(LOG_TRACE);

	/* Create ISPs */
	IISP *pISP = NULL;
	pP4P->CreateISP(&pISP);
	pISP->SetPortal((const unsigned char*)"p4p-8.cs.yale.edu", 6871);
	pISP->LoadP4PInfo();
	int pid;
	pISP->Lookup((const unsigned char*)"155.98.10.1", &pid);
	printf("[Test] 155.98.10.1@CT: %d\n", pid);
	
	/* Manager ISP */
	IISPManager *pISPManager = NULL;
	pP4P->CreateISPManager(&pISPManager);
	pISPManager->AddISP((const unsigned char*)"CT", pISP);	
	pISP->Release();

	/* Peering Guidance Matrix based peer selection manager */
	IPGMSelectionManager *pPSM = NULL;
	pP4P->CreatePGMSelectionManager(&pPSM);
	pPSM->SetAOE((const unsigned char*)"p4p-8.cs.yale.edu", 6673);
	pPSM->SetISPManager(pISPManager);
	pISPManager->Release();

	IPGMOptions *pPGMOptions = NULL;
	pP4P->CreatePGMOptions(&pPGMOptions);
	pPGMOptions->SetAlgorithm(PSA_LOCATION);
	pPGMOptions->SetLocationSelectionPercent(0.7, 0.9);
	pPSM->SetDefaultPGMOptions(pPGMOptions);
	pPSM->SetISPPGMOptions(pISP, pPGMOptions);

	pPSM->InitGuidance();

	/* Peer Join */
	IP4PPeer *pPeer = NULL;
	PEERINFO pinfo;
		
	pP4P->CreateP4PPeer(&pPeer);
	pinfo.PeerType = PT_PEER;
	pinfo.UploadCapacity = 32;
	pinfo.DownloadCapacity = 48;
	HRESULT ret = pPeer->SetPeerInfo((const unsigned char*)"202.114.13.2", pinfo);
	ret = pPSM->AddPeer(pPeer);
	pPeer->Release();

	pPeer = NULL;
	pP4P->CreateP4PPeer(&pPeer);
	pinfo.PeerType = PT_PEER;
	pinfo.UploadCapacity = 1024;
	pinfo.DownloadCapacity = 1024;
	ret = pPeer->SetPeerInfo((const unsigned char*)"155.98.10.1", pinfo);
	ret = pPSM->AddPeer(pPeer);
	pPeer->Release();

	/* Select P4PPeers */
	IP4PPeerList *pPeerList = NULL;
	ret = pPSM->SelectPeers(pPeer, 50, &pPeerList);	
	IP4PPeer *ptmp = NULL;
	unsigned int s;
	pPeerList->GetSize(&s);
	for (unsigned int i = 0; i < s; i++)
	{
		pPeerList->GetPeer(i, &ptmp);
		ptmp->GetPeerInfo(&pinfo);
		printf("[Test Peer Selection] Up: %f, Down: %f\n", pinfo.UploadCapacity, pinfo.DownloadCapacity);
		ptmp->Release();
	}
	pPeerList->Release();

	getchar();

	/* Peer Leave */
	pPSM->RemovePeer(pPeer);

	/* Finish */
	pPSM->Release();
	pP4P->Release();
	return 0;
}

