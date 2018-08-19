#ifndef __FONLINE_ANGELSCRIPT__
#define __FONLINE_ANGELSCRIPT__

// inlined in fonline_tla.cpp

bool Global_GetCurrentModule( ScriptString& moduleName )
{
	asIScriptContext* ctx = ::ScriptGetActiveContext();
	if( !ctx )
		return( false );

	asIScriptFunction* function = ctx->GetFunction();
	if( !function )
		return( false );

	moduleName = ScriptString::Create( function->GetModuleName() );

	return( true );
}

bool Global_GetCurrentFunction( ScriptString& moduleName, ScriptString& functionName, ScriptString& declaration, bool includeNamespace = false )
{
	asIScriptContext* ctx = ::ScriptGetActiveContext();
	if( !ctx )
		return( false );

	asIScriptFunction* function = ctx->GetFunction();
	if( !function )
		return( false );

	moduleName = ScriptString::Create( function->GetModuleName() );
	functionName = ScriptString::Create( function->GetName() );
	declaration = ScriptString::Create( function->GetDeclaration( true, includeNamespace ));

	return( true );
}

uint Global_GetEnumList_module( ScriptString& moduleName, ScriptArray* names )
{
	names->Resize(0);

	asIScriptModule* module = ASEngine->GetModule( moduleName.c_str(), asGM_ONLY_IF_EXISTS );
	if( !module )
		return( 0 );

	uint count = module->GetEnumCount();
	if( count <= 0 )
		return( count );

	names->Resize( count );

	for( uint e=0; e<count; e++ )
	{
		int enumTypeId;
		const char* enumName = module->GetEnumByIndex( e, &enumTypeId );
		ScriptString& outEnumName = ScriptString::Create( enumName );
		*((ScriptString**)(names->At(e))) = &outEnumName;
	}

	return( count );
}

uint Global_GetEnumList( ScriptArray* names )
{
	ScriptString& moduleName = ScriptString::Create( "" );
	if( Global_GetCurrentModule( moduleName ))
	{
		return( Global_GetEnumList_module( moduleName, names ));
	}

	return( 0 );
}

uint Global_GetEnum_module( ScriptString& moduleName, ScriptString& enumName, ScriptArray* names, ScriptArray* values )
{
	names->Resize(0);
	values->Resize(0);

	asIScriptModule* module = ASEngine->GetModule( moduleName.c_str(), asGM_ONLY_IF_EXISTS );
	if( !module )
		return( 0 );

	for( int e=0, elen=module->GetEnumCount(); e<elen; e++ )
	{
		int enumTypeId;
		const char* eName = module->GetEnumByIndex( e, &enumTypeId );

		if( !strcmp(enumName.c_str(), eName) )
		{
			int count = module->GetEnumValueCount( enumTypeId );
			if( count <= 0 )
				return( 0 );

			names->Resize( count );
			values->Resize( count );

			for( int v=0; v<count; v++ )
			{
				int outValue;
				const char* valueName = module->GetEnumValueByIndex( enumTypeId, v, &outValue );

				ScriptString& outValueName = ScriptString::Create( valueName );
				*((ScriptString**)(names->At(v))) = &outValueName;
				*((int*)(values->At(v))) = outValue;
			}
			return( count );
		}
	}

	return( 0 );
}

uint Global_GetEnum( ScriptString& enumName, ScriptArray* names, ScriptArray* values )
{
	ScriptString& moduleName = ScriptString::Create( "" );
	if( Global_GetCurrentModule( moduleName ))
	{
		return( Global_GetEnum_module( moduleName, enumName, names, values ));
	}

	return( 0 );
}

bool Global_EnumContains_module( ScriptString& moduleName, ScriptString& enumName, int value )
{
	asIScriptModule* module = ASEngine->GetModule( moduleName.c_str(), asGM_ONLY_IF_EXISTS );
	if( !module )
		return( false );

	for( int e=0, elen=module->GetEnumCount(); e<elen; e++ )
	{
		int enumTypeId;
		const char* eName = module->GetEnumByIndex( e, &enumTypeId );

		if( !strcmp(enumName.c_str(), eName) )
		{
			int count = module->GetEnumValueCount( enumTypeId );
			if( count <= 0 )
				return( false );

			for( int v=0; v<count; v++ )
			{
				int outValue;
				module->GetEnumValueByIndex( enumTypeId, v, &outValue );

				if( outValue == value )
					return( true );
			}
		}
	}

	return( false );
}

bool Global_EnumContains( ScriptString& enumName, int value )
{
	ScriptString& moduleName = ScriptString::Create( "" );
	if( Global_GetCurrentModule( moduleName ))
	{
		return( Global_EnumContains_module( moduleName, enumName, value ));
	}

	return( false );
}

uint Global_GetEnumValueCount_module( ScriptString& moduleName, ScriptString& enumName )
{
	asIScriptModule* module = ASEngine->GetModule( moduleName.c_str(), asGM_ONLY_IF_EXISTS );
	if( !module )
		return( 0 );

	for( int e=0, elen=module->GetEnumCount(); e<elen; e++ )
	{
		int enumTypeId;
		const char* eName = module->GetEnumByIndex( e, &enumTypeId );

		if( !strcmp(enumName.c_str(), eName) )
		{
			int count = module->GetEnumValueCount( enumTypeId );
			if( count <= 0 )
				return( 0 );
			return( count );
		}
	}

	return( 0 );
}

uint Global_GetEnumValueCount( ScriptString& enumName )
{
	ScriptString& moduleName = ScriptString::Create( "" );
	if( Global_GetCurrentModule( moduleName ))
	{
		return( Global_GetEnumValueCount_module( moduleName, enumName ));
	}

	return( 0 );
}

void RegisterAngelScriptExtensions()
{
	const char* fail = "Failed to register";

	if(ASEngine->RegisterGlobalFunction("bool GetCurrentModule(string& moduleName)", asFUNCTION(Global_GetCurrentModule), asCALL_CDECL) < 0)
		Log("%s GetCurrentModule()\n", fail);
	if(ASEngine->RegisterGlobalFunction("bool GetCurrentFunction(string& moduleName, string& functionName, string& declaration, bool includeNamespace = false)", asFUNCTION(Global_GetCurrentFunction), asCALL_CDECL) < 0)
		Log("%s GetCurrentFunction()\n", fail);
	if(ASEngine->RegisterGlobalFunction("uint GetEnum(string& enumName, string@[]@+ names, int[]@+ values)", asFUNCTION(Global_GetEnum), asCALL_CDECL) < 0)
		Log("%s GetEnum()\n", fail);
	if(ASEngine->RegisterGlobalFunction("uint GetEnum(string& moduleName, string& enumName, string@[]@+ names, int[]@+ values)", asFUNCTION(Global_GetEnum_module), asCALL_CDECL) < 0)
		Log("%s GetEnum(moduleName)\n", fail);
	if(ASEngine->RegisterGlobalFunction("uint GetEnumList(string@[]@+ names)", asFUNCTION(Global_GetEnumList), asCALL_CDECL) < 0)
		Log("%s GetEnumList()\n", fail);
	if(ASEngine->RegisterGlobalFunction("uint GetEnumList(string& moduleName, string@[]@+ names)", asFUNCTION(Global_GetEnumList_module), asCALL_CDECL) < 0)
		Log("%s GetEnumList(moduleName)\n", fail);
	if(ASEngine->RegisterGlobalFunction("bool EnumContains(string& enumName, int value)", asFUNCTION(Global_EnumContains), asCALL_CDECL) < 0)
		Log("%s EnumContains()\n", fail);
	if(ASEngine->RegisterGlobalFunction("bool EnumContains(string& moduleName, string& enumName, int value)", asFUNCTION(Global_EnumContains_module), asCALL_CDECL) < 0)
		Log("%s EnumContains(moduleName)\n", fail);
	if(ASEngine->RegisterGlobalFunction("uint GetEnumValueCount(string& enumName)", asFUNCTION(Global_GetEnumValueCount), asCALL_CDECL) < 0 )
		Log("%s GetEnumValueCount()\n", fail);
	if(ASEngine->RegisterGlobalFunction("uint GetEnumValueCount(string& moduleName, string& enumName)",asFUNCTION(Global_GetEnumValueCount_module), asCALL_CDECL) < 0 )
		Log("%s GetEnumValueCount(moduleName)\n", fail);
}

#endif // __FONLINE_ANGELSCRIPT__ //
