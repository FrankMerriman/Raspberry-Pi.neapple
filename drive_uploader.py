from googleapiclient.http import MediaFileUpload
import os.path
import os
from googleapiclient.discovery import build
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request
from google.oauth2.credentials import Credentials
 
SECRET_FILE = 'token.json'
API_NAME = 'drive'
API_VERSION = 'v3'
SCOPES = ['https://www.googleapis.com/auth/drive']
 
creds = None
# The file token.json stores the user's access and refresh tokens, and is
# created automatically when the authorization flow completes for the first
# time.
if os.path.exists('token.json'):
    creds = Credentials.from_authorized_user_file('token.json', SCOPES)
# If there are no (valid) credentials available, let the user log in.
if not creds or not creds.valid:
    if creds and creds.expired and creds.refresh_token:
        creds.refresh(Request())
    else:
        flow = InstalledAppFlow.from_client_secrets_file(
            'credentials.json', SCOPES)
        creds = flow.run_local_server(port=0)
    # Save the credentials for the next run
    with open('token.json', 'w') as token:
        token.write(creds.to_json())
 
service = build('drive', 'v3', credentials=creds)

#Infinite loop to grab all files that currently exist in the complete_dumps dir
path = os.getcwd()

while True:
	#make this section grab files from dir in final
	file_names = os.listdir(path+'/complete_dumps')
	mime_type = 'application/vnd.tcpdump.pcap'

	for file_name in file_names:
		print('uploading file: {0}'.format(file_name))
		file_metadata = {
			'name': file_name
		}
	 
		media = MediaFileUpload('./complete_dumps/{0}'.format(file_name), mimetype=mime_type)
	 
		service.files().create(
			body=file_metadata,
			media_body=media,
			fields='id'
		).execute()
		
		#Remove file after upload
		print('deleting file: {0}'.format(file_name))
		#os.remove(path+'/complete_dumps/'+filename)
		#if someone was able to modify filename this would be a boo boo
		os.system('sudo rm -r complete_dumps/'+'"'+file_name+'"')
