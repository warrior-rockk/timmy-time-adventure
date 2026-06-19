entity_destroy:

//update id
entityList[entityIndex].id = entityIndex;

entity_update:
if (entityList[i].id != i)
	MY_TRACE_FLAG("ERROR: entity id %i on position array %i\n", entityList[i].id, i);

object_destroy:
	update object instance number   
    TODO: what happens with platform or trigger?? 
    tEntity *lastEntityObject = entity_get_by_instance(E_ENT_CLASS_OBJECT, numObjectInstances - 1);
    ASSERT(lastEntityObject);
    lastEntityObject->entInstance = objectIndex;