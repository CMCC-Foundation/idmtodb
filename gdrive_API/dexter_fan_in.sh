#!/bin/bash

notify="$1"
out_dir_name="$2"
docx_filename_out="$3"
name_dot_surname="$4"
email="$5"
issuer="$6"
first="$7"
username="$8"
div="$9"
mach="${10}"

cd /root/gdrive_API
source gdrive_venv/bin/activate;
mkdir -p files/$out_dir_name ;
mv files/$(echo $docx_filename_out | cut -d'/' -f2) files/$docx_filename_out
python3 upload_docx_convert_to_pdf.py files/$docx_filename_out $name_dot_surname $email $issuer >> files/$out_dir_name/drive_api_log
cat files/$out_dir_name/drive_api_log

if [[ "$notify" -eq 1 ]] || [[ ! -z "$notify" ]];
then
	./send_user_mail.sh "$(echo ${issuer:0:1} | tr '[:lower:]' '[:upper:]')""$(echo ${issuer:1:${#issuer}} | cut -d'_' -f1)" "$first" "$username" "$email" "$(echo $div | tr '[:lower:]' '[:upper:]')" "$(cat files/$out_dir_name/drive_api_log | grep 'Folder link' | tail -n1 | cut -d' ' -f3 | tr -d '\n')" "files/$out_dir_name/$(echo $docx_filename_out | cut -d'/' -f2 | cut -d'.' -f1).pdf" "$mach"
fi
