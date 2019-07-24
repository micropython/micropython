/*
 * Copyright (C) OASIS Open 2014.  All rights reserved.
 * OASIS trademark, IPR and other policies apply.
 * http://www.oasis-open.org/policies-guidelines/ipr
 */

#ifndef _PKCS11_SE_H_
#define _PKCS11_SE_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

/* C_Initialize initializes the Cryptoki library. */
CK_RV C_Initialize_sm(CK_VOID_PTR pInitArgs);



/* C_Finalize indicates that an application is done with the
 * Cryptoki library. */
CK_RV C_Finalize_sm(CK_VOID_PTR pvReserved);

/* C_GetFunctionList returns the function list. */
CK_RV C_GetFunctionList_sm(CK_FUNCTION_LIST_PTR_PTR ppFunctionList);

/* Session management */

/* C_OpenSession opens a session between an application and a
 * token. */
CK_RV C_OpenSession_sm(  CK_SLOT_ID            slotID,        /* the slot's ID */
                         CK_FLAGS              flags,         /* from CK_SESSION_INFO */
                         CK_VOID_PTR           pApplication,  /* passed to callback */
                         CK_NOTIFY             Notify,        /* callback function */
                         CK_SESSION_HANDLE_PTR phSession      /* gets session handle */
                      );



/* C_CloseSession closes a session between an application and a
 * token. */
CK_RV C_CloseSession_sm ( CK_SESSION_HANDLE xSession );


/* C_GetAttributeValue obtains the value of one or more object
 * attributes. */
CK_RV C_GetAttributeValue_sm(  CK_SESSION_HANDLE hSession,   /* the session's handle */
                               CK_OBJECT_HANDLE  hObject,    /* the object's handle */
                               CK_ATTRIBUTE_PTR  pTemplate,  /* specifies attrs; gets vals */
                               CK_ULONG          ulCount     /* attributes in template */
                            );

/* C_FindObjectsInit initializes a search for token and session
 * objects that match a template. */
 CK_RV C_FindObjectsInit_sm ( CK_SESSION_HANDLE xSession,
                                                CK_ATTRIBUTE_PTR pxTemplate,
                                                CK_ULONG ulCount );

 /* C_FindObjects continues a search for token and session
  * objects that match a template, obtaining additional object
  * handles. */
 CK_RV C_FindObjects_sm ( CK_SESSION_HANDLE xSession,
                                             CK_OBJECT_HANDLE_PTR pxObject,
                                             CK_ULONG ulMaxObjectCount,
                                             CK_ULONG_PTR pulObjectCount );


 /* C_FindObjectsFinal finishes a search for token and session
  * objects. */
 CK_RV  C_FindObjectsFinal_sm ( CK_SESSION_HANDLE xSession );


 /**
  * @brief Terminate object enumeration.
  */
 CK_RV  C_FindObjectsFinal_sm ( CK_SESSION_HANDLE xSession );

#ifdef __cplusplus
}
#endif

#endif /*_PKCS11_SE_H_*/
