// file PWScore.cpp
//-----------------------------------------------------------------------------

#include <fstream.h> // for WritePlaintextFile

#include "PWScore.h"
#include "global.h"

PWScore::PWScore() : m_currfile(_T("")), m_changed(false),
		     m_usedefuser(false), m_defusername(_T("")),
		     m_ReadFileVersion(PWSfile::UNKNOWN_VERSION)
{
}

PWScore::~PWScore()
{
  // Nothing (for now)
}

void
PWScore::ClearData(void)
{
  global.m_passkey.Trash();

   //Composed of ciphertext, so doesn't need to be overwritten
   m_pwlist.RemoveAll();
	
}

void
PWScore::NewFile(const CMyString &passkey)
{
   ClearData();
   global.m_passkey = passkey;
   m_changed = false;
}

int
PWScore::WriteFile(const CMyString &filename, PWSfile::VERSION version)
{
  PWSfile out(filename, global.m_passkey);

  int status;

  status = out.OpenWriteFile(version);

  if (status != PWSfile::SUCCESS)
    return CANT_OPEN_FILE;

  //Write out full names
  if (GetUseDefUser())
    MakeFullNames(GetDefUsername());

  CItemData temp;
  POSITION listPos = m_pwlist.GetHeadPosition();
  while (listPos != NULL)
    {
      temp = m_pwlist.GetAt(listPos);
      out.WriteRecord(temp);
      m_pwlist.GetNext(listPos);
    }
  out.CloseFile();


  //Restore shortened names if necessary
  if (GetUseDefUser())
    DropDefUsernames(GetDefUsername());

  m_changed = FALSE;
  m_ReadFileVersion = version; // needed when saving a V17 as V20 1st time [871893]
  return SUCCESS;
}

int
PWScore::WritePlaintextFile(const CMyString &filename)
{
  ofstream of(filename);

  if (!of)
    return CANT_OPEN_FILE;

  //Write out full names
  if (GetUseDefUser())
    MakeFullNames(GetDefUsername());

  CItemData temp;
  POSITION listPos = m_pwlist.GetHeadPosition();
  while (listPos != NULL)
    {
      temp = m_pwlist.GetAt(listPos);
      of << temp.GetPlaintext('\t') << endl;
      m_pwlist.GetNext(listPos);
    }
  of.close();


  //Restore shortened names if necessary
  if (GetUseDefUser())
    DropDefUsernames(GetDefUsername());

  return SUCCESS;
}

int PWScore::CheckPassword(const CMyString &filename, CMyString& passkey)
{
  PWSfile f(filename, passkey);

  int status = f.CheckPassword();

  switch (status) {
  case PWSfile::SUCCESS:
    return SUCCESS;
  case PWSfile::CANT_OPEN_FILE:
    return CANT_OPEN_FILE;
  case PWSfile::WRONG_PASSWORD:
    return WRONG_PASSWORD;
  default:
    ASSERT(0);
    return status; // should never happen
  }
}


int
PWScore::ReadFile(const CMyString &a_filename,
                   const CMyString &a_passkey)
{	
   //That passkey had better be the same one that came from CheckPassword(...)

   PWSfile in(a_filename, a_passkey);

  int status;
  
  m_ReadFileVersion = in.GetFileVersion();

  if (m_ReadFileVersion == PWSfile::UNKNOWN_VERSION)
    return UNKNOWN_VERSION;

  status = in.OpenReadFile(m_ReadFileVersion);

  if (status != PWSfile::SUCCESS)
    return CANT_OPEN_FILE;

   ClearData(); //Before overwriting old data, but after opening the file... 

   global.m_passkey = a_passkey;

   CItemData temp;

   status = in.ReadRecord(temp);

   while (status == PWSfile::SUCCESS)
   {
      m_pwlist.AddTail(temp);
      status = in.ReadRecord(temp);
   }

   in.CloseFile();

   //Shorten names if necessary
   if (GetUseDefUser())
   {
      DropDefUsernames(GetDefUsername());
   }

   return SUCCESS;
}

int PWScore::RenameFile(const CMyString &oldname, const CMyString &newname)
{
  return PWSfile::RenameFile(oldname, newname);
}


void PWScore::ChangePassword(const CMyString &newPassword)
{
 
  POSITION listPos = m_pwlist.GetHeadPosition();
  while (listPos != NULL)
    {
      m_pwlist.GetAt(listPos).ChangePassKey(global.m_passkey, newPassword);
      m_pwlist.GetNext(listPos);
    }
  //Changes the global password. Eck.
  global.m_passkey = newPassword;
  m_changed = TRUE;
}


// Finds stuff based on title & user fields only
POSITION
PWScore::Find(const CMyString &a_title, const CMyString &a_user)
{
   POSITION listPos = m_pwlist.GetHeadPosition();
   CMyString title, user;

   while (listPos != NULL)
   {
      title = m_pwlist.GetAt(listPos).GetTitle();
      user = m_pwlist.GetAt(listPos).GetUser();
      if (title == a_title && user == a_user)
         break;
      else
         m_pwlist.GetNext(listPos);
   }

   return listPos;
}


/*
  The following two functions are for use when switching default
  username states.

  Should be run only if m_usedefuser == TRUE
*/
void
PWScore::MakeFullNames(const CMyString &defusername)
{
  POSITION listPos = m_pwlist.GetHeadPosition();
  CMyString temp;
  while (listPos != NULL)
    {
      temp = m_pwlist.GetAt(listPos).GetName();
      //Start MakeFullName
      int pos = temp.FindByte(SPLTCHR);
      int pos2 = temp.FindByte(DEFUSERCHR);
      if (pos==-1 && pos2!=-1)
	{
	  //Insert defusername if string contains defchr but not splitchr
	  m_pwlist.GetAt(listPos).SetName((CMyString)temp.Left(pos2)
					  + SPLTSTR + defusername);
	}
      // End MakeFullName
      m_pwlist.GetNext(listPos);
    }
}

//Should only be run on full names...
void
PWScore::DropDefUsernames(const CMyString &defusername)
{
  POSITION listPos = m_pwlist.GetHeadPosition();
  CMyString temp;
  while (listPos != NULL)
    {
      temp = m_pwlist.GetAt(listPos).GetName();
      //Start DropDefUsername
      CMyString temptitle, tempusername;
      int pos = SplitName(temp, temptitle, tempusername);
      if ((pos!=-1) && (tempusername == defusername))
	{
	  //If name is splitable and username is default
	  m_pwlist.GetAt(listPos).SetName(temptitle + DEFUSERCHR);
	}
      //End DropDefUsername
      m_pwlist.GetNext(listPos);
    }
}


// XXX these should be moved to ItemData - except for Default Name handling!

int
PWScore::SplitName(const CMyString &name,
		   CMyString &title, CMyString &username)
//Returns split position for a name that was split and -1 for non-split name
{
   int pos = name.FindByte(SPLTCHR);
   if (pos==-1) //Not a split name
   {
      int pos2 = name.FindByte(DEFUSERCHR);
      if (pos2 == -1)  //Make certain that you remove the DEFUSERCHR 
      {
         title = name;
      }
      else
      {
         title = CMyString(name.Left(pos2));
      }

      if ((pos2 != -1)
          && GetUseDefUser())
      {
         username = GetDefUsername();
      }
      else
      {
         username = _T("");
      }
   }
   else
   {
      /*
       * There should never ever be both a SPLITCHR and a DEFUSERCHR in
       * the same string
       */
      CMyString temp;
      temp = CMyString(name.Left(pos));
      temp.TrimRight();
      title = temp;
      temp = CMyString(name.Right(name.GetLength() - (pos+1))); // Zero-index string
      temp.TrimLeft();
      username = temp;
   }
   return pos;
}


void
PWScore::MakeName(CMyString& name,
		  const CMyString &title, const CMyString &username) const
{
   if (username == "")
      name = title;
   else if (GetUseDefUser()
	    && (username == GetDefUsername()))
   {
      name = title + DEFUSERCHR;
   }
   else 
   {
      name = title + SPLTSTR + username;
   }
}
