/*
** ----------------------------------------------------------------------
**
** FILE:	Vegetation Model
**
**  (C-) Josep Jorba, 1997		C.A.O.S. - UAB
** 
** ----------------------------------------------------------------------
**
** Date			Who		What
** 26-01-98		JJ		Creation
**
** ----------------------------------------------------------------------
*/

#ifndef _VEGETMOD_H_
#define _VEGETMOD_H_

/* Maxim numero  de praametres d'un model de vegetacio */
#define MAX_PARAMS 9
/*
** Estructures que defineixen els models de vegetacio
*/
/* Definicio d'una submodel de vegetacio */
typedef struct{
  int submodelCode;         /* Codi de submodel */
  double params[MAX_PARAMS]; /* Parametres del submodel */
} vegetSubmodel;
/* Model de vegetacio : conjunt de submodels */
typedef struct{
  int nSubmodels;             /* Numero de submodels que tenim */
  vegetSubmodel* submodels; /* submodels */
} vegetModels;             /* Models de vegetacio */

typedef vegetModels* pvegetModels;

/* Vegetation functions */
void veget_setup(pvegetModels* v, char* vegetFile);
void veget_unsetup(pvegetModels* v);
void veget_statistics(pvegetModels* v);
void veget_readVegetFile(pvegetModels* v, char* name);

/*
//  MPI DataType for vegent submodel

int veget_models_create_datatypes ()
{
	MPI_Datatype vegetSubmodelT;
	
	int blocks [] =  { 1, MAX_PARAMS } ;
	int disp [] =  { 0, sizeof (int) } ;
	MPI_Datatype types [] = { MPI_INT, paramsT };
	
	MPI_Type_create_struct (2, blocks, disp, types, &vegetSubmodelT);

	MPI_Type_create_struct (2, blocks, disp, types, &vegetSubmodelT);
*/


#endif /* _VEGETMOD_H_ */
