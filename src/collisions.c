#include <stdint.h>
#include "globals.h"
#include "collisions.h"

int16_t colCheckTileTerrain(tEntity idEntity,int i)
{ 

    tVector colVector;	//Vector de comprobacion colision
    int distColX;		//Distancia con la colision en X
    int distColY;		//Distancia con la colision en Y
    int colDir;			//Sentido de la colision

    /*
    colDir = 0;
            
    //comprobamos si el punto de control esta activo
    if (!idEntity.this.colPoint[i].enabled) return colDir; end;
    
    //===============
    //COLISIONES EN X
    //===============
    
    //desactivamos puntos de control inferiores si estamos en rampa
    if (cSlopesEnabled)
        idEntity.this.colPoint[LEFT_DOWN_POINT].enabled  = getTileCode(idEntity,CENTER_DOWN_POINT) <> SLOPE_135;
        idEntity.this.colPoint[RIGHT_DOWN_POINT].enabled = getTileCode(idEntity,CENTER_DOWN_POINT) <> SLOPE_45;
    end;
    
    //si el punto de deteccion es lateral (X)
    if (idEntity.this.colPoint[i].colCode == COLDER || idEntity.this.colPoint[i].colCode == COLIZQ )
        
        //Establecemos el vector a chequear
        colVector.vStart.x = idEntity.this.fX+idEntity.this.colPoint[i].x;
        colVector.vEnd.x   = colVector.vStart.x+idEntity.this.vX;
        colVector.vStart.y = idEntity.this.fY+idEntity.this.colPoint[i].y;
        colVector.vEnd.y   = colVector.vStart.y;
            
        //lanzamos la comprobacion de colision en X
        distColX = colCheckVectorX(idEntity,&colVector,idEntity.this.colPoint[i].colCode);
        
        //Si hay colision
        If (distColX>=0)
            //Colision Derecha
            if (idEntity.this.colPoint[i].colCode == COLDER) 
                //situamos el objeto al borde de la colision	
                idEntity.this.fX+= distColX-1;
                colDir = COLDER;
                
            end;
            //Colision Izquierda
            if (idEntity.this.colPoint[i].colCode == COLIZQ) 			
                //situamos el objeto al borde de la colision
                idEntity.this.fX-= distColX-1;
                colDir = COLIZQ;
            end;
        end;  
    end;
    
    //===============
    //COLISIONES EN Y
    //===============
    
    //Si el punto de deteccion es uno de los superiores/inferiores
    if (idEntity.this.colPoint[i].colCode == COLUP || idEntity.this.colPoint[i].colCode == COLDOWN)
        
        //Establecemos el vector a comparar
        colVector.vStart.x = idEntity.this.fX+idEntity.this.colPoint[i].x;
        colVector.vEnd.x   = colVector.vStart.x;
        colVector.vStart.y = idEntity.this.fY+idEntity.this.colPoint[i].y;
        colVector.vEnd.y   = colVector.vStart.y+idEntity.this.vY;
        
        //Lanzamos la comprobacion de colision en Y
        distColY = colCheckVectorY(idEntity,&colVector,idEntity.this.colPoint[i].colCode,TOCOLLISION);
        
        //Si hay colision
        If (distColY>=0) 
            //Colision inferior
            if (idEntity.this.colPoint[i].colCode == COLDOWN && idEntity.this.vY>=0)
                //Situamos al objeto en el borde de la colision
                idEntity.this.fY += distColY;
                colDir = COLDOWN;
                
                //Deteccion de pendiente,comprobamos si estamos enterrados
                if (cSlopesEnabled)
                                            
                    //Establecemos el vector a comparar (centro/inferior del objeto)
                    colVector.vStart.x = idEntity.this.fX+idEntity.this.colPoint[CENTER_DOWN_POINT].x;
                    colVector.vEnd.x   = colVector.vStart.x;
                    colVector.vStart.y = idEntity.this.fY+idEntity.this.colPoint[CENTER_DOWN_POINT].y;
                    colVector.vEnd.y   = colVector.vStart.y-cHillHeight; //altura maxima para considerar pendiente
                    
                    //Lanzamos la comprobacion de colision en Y
                    distColY = colCheckVectorY(idEntity,&colVector,COLCENTER,FROMCOLLISION);
                    
                    //Subimos al objeto a la pendiente
                    if (distColY >0)
                        idEntity.this.fY -= distColY-1;
                    end;
                end;
            End;                                 
            
            //Colision superior
            if (idEntity.this.colPoint[i].colCode == COLUP && idEntity.this.vY<0)
                //Situamos al objeto en el borde de la colision
                idEntity.this.fY -= distColY;
                colDir = COLUP;
            End;
        
        else 
            //si no hay colision, comprobamos si pendiente hacia abajo
            if (cSlopesEnabled)
                //lo comprobamos si no estamos en escalera para despegarnos del suelo
                if (idEntity.this.vY > 0)
                    //Establecemos el vector a comparar (centro/inferior del objeto)
                    colVector.vStart.x = idEntity.this.fX+idEntity.this.colPoint[CENTER_DOWN_POINT].x;
                    colVector.vEnd.x   = colVector.vStart.x;
                    colVector.vStart.y = idEntity.this.fY+idEntity.this.colPoint[CENTER_DOWN_POINT].y-1;
                    colVector.vEnd.y   = colVector.vStart.y+idEntity.this.vY+cHillHeight; //altura maxima para considerar pendiente
                    
                    //Lanzamos la comprobacion de colision en Y
                    distColY = colCheckVectorY(idEntity,&colVector,COLCENTER,TOCOLLISION);
                    
                    //Bajamos al objeto a la pendiente
                    if (distColY >0)
                        idEntity.this.fY += distColY;
                    end;	
                end;
            end;
        end; 
        
    end;
    
    //Devolvemos el sentido de la colision
    return colDir;
    */
   return 1;
}