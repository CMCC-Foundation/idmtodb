import os.path
import sys
import io

from gdrive_API import *
from googleapiclient.discovery import build

#import google.auth
#from googleapiclient.discovery import build
#from googleapiclient.errors import HttpError

# If modifying these scopes, delete the file token.json.
#SCOPES = ["https://www.googleapis.com/auth/drive", "https://www.googleapis.com/auth/drive.metadata.readonly"]

real_file_name = sys.argv[1]
real_folder_name = sys.argv[2]
real_email = sys.argv[3]
real_parent_id = "10N_pqAKbry-xwOmjWvljKbIXR8KyosEL" # SCC-HSM/CMCC_IDM_users_credentials
real_token = sys.argv[4] #"marco_chiarelli"

print("-------------------------------------------")
print("Real file name: {}".format(real_file_name))
print("Real folder name: {}, Real email: {}".format(real_folder_name, real_email))
print("Real parent ID: {}".format(real_parent_id))
print("Real token: {}".format(real_token))

if __name__ == "__main__":
    try:
        service = build("drive", "v3", credentials=manage_creds(real_token))
        folder = create_folder(service, real_folder_name, parent=real_parent_id)
        folder_id = folder.get("id")
        folder_link = folder.get("webViewLink")
        print("Folder link: {}".format(folder_link))
        file_id, file_name = upload_to_folder(service, folder_id=folder_id, file_name=real_file_name, out_file_name=real_file_name.split("/")[-1], mimeTypeIn="application/vnd.openxmlformats-officedocument.wordprocessingml.document", mimeTypeOut="application/vnd.google-apps.document")
        bytesio_object, file_name = download_file(service, file_id, file_name, mimeType="application/pdf")
        # Write the stuff
        out_file_name = file_name.split(".")[0]+".pdf"
        
        with open(out_file_name, "wb") as f:
            f.write(bytesio_object.getbuffer())
            
        service.files().delete(fileId=file_id).execute()
        upload_to_folder(service, folder_id=folder_id, file_name=out_file_name, out_file_name=out_file_name.split("/")[-1], mimeTypeIn="application/pdf", mimeTypeOut="application/pdf")
        
        mail_domain=real_email.split("@")[1].split(".")[0]
        if(real_email and (mail_domain == "cmcc" or mail_domain == "gmail" or mail_domain == "eiee")):
            user_permission = {
                "type": "user",
                "role": "reader",
                "emailAddress": real_email,
            }
            
            perm = (service.permissions().create(fileId=folder_id, sendNotificationEmail = False, body = user_permission, fields="id").execute())
            print(f'Permission ID: "{perm.get("id")}".')
            
    except HttpError as error:
        print(f"An error occurred: {error}")
  
print("-------------------------------------------")
