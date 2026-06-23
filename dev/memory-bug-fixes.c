object_destroy:
	update object instance number   
    TODO: what happens with platform or trigger?? 
    tEntity *lastEntityObject = entity_get_by_instance(E_ENT_CLASS_OBJECT, numObjectInstances - 1);
    ASSERT(lastEntityObject);
    lastEntityObject->entInstance = objectIndex;