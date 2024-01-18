DELIMITER ;; 
DROP FUNCTION IF EXISTS select_division_director_mail;
CREATE FUNCTION select_division_director_mail
(
     division_name VARCHAR(32)    
)  
RETURNS VARCHAR(64) DETERMINISTIC
   BEGIN
	DECLARE result VARCHAR(64);
	SELECT SPLIT_STRING('Director: ',idmdb.group.desc,2) INTO result FROM idmdb.group WHERE name=division_name;
  	RETURN result;
   END 
   ;;

