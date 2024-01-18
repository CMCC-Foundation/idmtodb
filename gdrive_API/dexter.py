import os.path
import sys
import io

from gdrive_API import manage_creds, download_file, upload_to_folder #*
from googleapiclient.discovery import build
from dexter_idm_fileid import dexter_idm_fileid

#import google.auth
#from googleapiclient.discovery import build
#from googleapiclient.errors import HttpError

# If modifying these scopes, delete the file token.json.
#SCOPES = ["https://www.googleapis.com/auth/drive", "https://www.googleapis.com/auth/drive.metadata.readonly"]
#GOOGLE_DRIVE_DEXTER_IDM_FILEID="1PR6Fsc6C7EuUCIOskBnQbSUx3OecxhnI8nnaF553TTE"

out_file_name = "IDM_Dexter_users_list"

real_file_name = sys.argv[1]
real_parent_id = "1-GsY8AKk0b0j03pelh3Edie1MoqtnXUi" #"10N_pqAKbry-xwOmjWvljKbIXR8KyosEL" # SCC-HSM/CMCC_IDM_users_credentials
real_token = sys.argv[2] #"marco_chiarelli"

print("-------------------------------------------")
print("Real file name: {}".format(real_file_name))
#print("Real folder name: {}, Real email: {}".format(real_folder_name, real_email))
print("Real parent ID: {}".format(real_parent_id))
print("Real token: {}".format(real_token))

if __name__ == "__main__":
    try:
        service = build("drive", "v3", credentials=manage_creds(real_token))
        if(real_file_name.split(".")[-2].split("_")[-1] == "OUT"):
            bytesio_object, file_name = download_file(service, dexter_idm_fileid, real_file_name, mimeType="text/csv")

            with open(file_name, "wb") as f:
                f.write(bytesio_object.getbuffer())
        else:
            #service = build("drive", "v3", credentials=creds)
            
            #folder = create_folder(real_folder_name, parent=real_parent_id)
            #folder_id = folder.get("id")
            #folder_link = folder.get("webViewLink")
            #print("Folder link {}".format(folder_link))
            folder_id = real_parent_id
            service.files().delete(fileId=dexter_idm_fileid).execute()
            file_id, file_name = upload_to_folder(service, folder_id=folder_id, file_name=real_file_name, out_file_name=out_file_name, mimeTypeIn="text/csv", mimeTypeOut="application/vnd.google-apps.spreadsheet")
            print("dexter_idm_fileid=\"{}\"".format(file_id))            
    except HttpError as error:
        print(f"An error occurred: {error}")
  
print("-------------------------------------------")
