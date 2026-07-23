486@66mhz:

fps platform persistent (20 visible entities):		34~35
fps platforms no persistent (7 visible entities): 	35


jurassic 56 entities: 70fps
jurassic 56 entities all background tiles: 35fps

medieval: 138 ent 							(34-35fps)
medieval: 138 ent without map_draw 			(60~70fps) <-
medieval: 138 ent without ent_draw 			(33~35fps)
medieval: 138 ent without ent_update 		(60~70fps) <-
medieval: 138 ent without map_draw front	(32~35fps) 
medieval: 138 ent map blit instead drawSpr	(47~50fps) <-relevant but drops to 33-35 when play

egypt: 173 ent


why items persistent?


860 862