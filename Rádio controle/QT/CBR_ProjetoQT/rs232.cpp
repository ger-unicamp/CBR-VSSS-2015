#include "rs232.h"
#include <stdio.h>

RS232::RS232(const char *Port,const char *Parite,int Vitesse,int Data,const char *StopBit,int TimeOut)
{
    mutex.lock();

    taille_donnee = 20;

    DWORD dwError;					// n� de l'erreur
    BOOL flag_etat;					// tout c'est bien pass�
    wchar_t *pwc      = (wchar_t *)malloc( sizeof( wchar_t ));

/*--------------------------------------------------------*/
/*                 Ouverture du port de Com               */
/*--------------------------------------------------------*/

mbstowcs(pwc, Port, 5 );

        port_handle = CreateFile(
                pwc,                         	// Choix du port � COM �
                GENERIC_READ | GENERIC_WRITE, 	// acc�s pour lire et �crire sur le port
                0,                            	// acc�s exclusif au port de COM
                NULL,                         	// s�curit� par d�faut
                OPEN_EXISTING,                	// Doit �tre � cette valeur car ce n'est pas un fichier
                0,              		// mode asynchrone
                NULL);

/*-----------------------------------------------------------*/
/*         V�rifier si handle ouvert correctement            */
/*-----------------------------------------------------------*/
        if (port_handle == INVALID_HANDLE_VALUE)
                {
                        dwError = GetLastError();
                        /* Fichier non cr�er g�rer l'erreur */
                }

/*-----------------------------------------------------------*/
/* Lecture Configuration initiale                            */
/*-----------------------------------------------------------*/
        creation_ok = GetCommState(port_handle, &configuration);

/*-------------------------------------------------------------------*/
/* 					Configuration du port                            */
/*-------------------------------------------------------------------*/

// Gestion de la vitesse
        configuration.BaudRate = Vitesse;

// Gestion du nombre de bits
        configuration.ByteSize = Data;

// Gestion de la parit�
        if (strcmp(Parite,"Aucune")==0)
                configuration.Parity = NOPARITY;

        if (strcmp(Parite,"Paire")==0)
                configuration.Parity = EVENPARITY;

        if (strcmp(Parite,"Impaire")==0)
                configuration.Parity = ODDPARITY;

// Gestion du Stop Bit
        if (strcmp(StopBit,"2")==0)
                configuration.StopBits = TWOSTOPBITS;

        else if (strcmp(StopBit,"1.5")==0)
                configuration.StopBits = ONE5STOPBITS;

        else
                configuration.StopBits = ONESTOPBIT;

// configuration.configurationlength;

        configuration.fBinary = 1;
        configuration.fParity = 0;
        configuration.fOutxCtsFlow = 0;
        configuration.fOutxDsrFlow = 0;
        configuration.fDtrControl = 0;
        configuration.fDsrSensitivity = 0;
        configuration.fTXContinueOnXoff = 0;
        configuration.fRtsControl = 0;

 /*----------------------------------*/
 /*    D�finition des timeouts       */
 /*----------------------------------*/

        temps_attente.ReadIntervalTimeout = MAXWORD;
        temps_attente.ReadTotalTimeoutMultiplier = 0;
        temps_attente.ReadTotalTimeoutConstant = TimeOut; // pas de time out = 0
        temps_attente.WriteTotalTimeoutMultiplier = 0;
        temps_attente.WriteTotalTimeoutConstant = 0;

// configurer le timeout

        SetCommTimeouts(port_handle,&temps_attente);

/*-----------------------------------------------*/
/* Configurer le port	                          */
/*-----------------------------------------------*/
        flag_etat = SetCommState(port_handle, &configuration);

        mutex.unlock();
}

int RS232::envoi_chaine(const char *Chaine,const char *EolChar)
{
        BOOL flag_etat;
        int nb_octets;
        char *Message;
        unsigned long nb_octets_wr;

        Message = new char[taille_donnee];

 /*-------------------------------------------------
          Envoi de la chaine + caractere de stop
 --------------------------------------------------*/
        if(strcmp(EolChar,"CR")==0)
                sprintf(Message,"%s%c",Chaine,0x0D);

        else if(strcmp(EolChar,"LF")==0)
                sprintf(Message,"%s%c",Chaine,0x0A);

        else if(strcmp(EolChar,"LF/CR")==0)
                sprintf(Message,"%s%c%c",Chaine,0x0A,0x0D);

        else if(strcmp(EolChar,"CR/LF")==0)
                sprintf(Message,"%s%c%c",Chaine,0x0D,0x0A);

        else
            sprintf(Message,"%s%c",Chaine,'$');

// compter le nombre d'octets a envoyer
        nb_octets = strlen(Message);

// ecrire dans le fichier
        mutex.lock();
        flag_etat = WriteFile(port_handle,Message,nb_octets,&nb_octets_wr,NULL);
        mutex.unlock();

// Liberation memoire
        delete[] Message;

        return(flag_etat);
}



int RS232::recevoir(char *ChaineRecue)
{
        BOOL flag_etat;
        unsigned long nBytesRead;
        char *buffer;

 /*-------------------------------------*/
 /* Initialisation des variables        */
 /*-------------------------------------*/

        buffer  = new char[taille_donnee-1];
        buffer[0] = '\0';
        nBytesRead=0;

 /*-------------------------------------------------------------*/
 /* boucle d'attente de lecture des octets                      */
 /* Sortie de la boucle par timeout par exemple, si             */
 /*-------------------------------------------------------------*/
        mutex.lock();
        flag_etat = ReadFile(port_handle,buffer,taille_donnee,&nBytesRead,NULL);
        mutex.unlock();

        buffer[nBytesRead] = '\0';          //Car pas forc�ment renvoy� par le syst�me reli� sur le port s�rie

        sprintf(ChaineRecue,"%s",buffer);   // Retourner la chaine recue

// Liberer la memoire
        delete[] buffer;

        return(flag_etat);
}



