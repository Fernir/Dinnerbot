enum CCharCreateInfo_Globals 
{
	dword_B6B1A0 = 0xB6B1A0,
	dword_AC4220 = 0xAC4220,
};

void CreateCharacter(const char *charName)
{
	/* SStrCopy(&dest, namePtr, 48);
		v10 = *(_BYTE *)(dword_B6B1A0 + 24);
		v4 = *(_BYTE *)(dword_B6B1A0 + 28);
		v11 = dword_AC4220;
		v12 = v4;
		v13 = *(_BYTE *)(dword_B6B1A0 + 40);
		v16 = *(_BYTE *)(dword_B6B1A0 + 36);
		v15 = *(_BYTE *)(dword_B6B1A0 + 52);
		v17 = *(_BYTE *)(dword_B6B1A0 + 48);
		v14 = *(_BYTE *)(dword_B6B1A0 + 44);
		v18 = 0;
		if ( v19 | v1 && v2 )
		{
			v5 = *(_DWORD *)(v2 + 372);
			if ( v5 & 0x100000 )
			{
				CCharCreateInfo::SendRaceChange(__PAIR__(v19, v1), &dest);
			}
			else
			{
				if ( v5 & 0x10000 )
					CCharCreateInfo::SendFactionChange(__PAIR__(v19, v1), &dest);
				else
					sub_4D8E10(__PAIR__(v19, v1), &dest);
			}
		}
		else
		{
			CGlueMgr::CreateCharacter((int)&dest);
		}
	}*/
}