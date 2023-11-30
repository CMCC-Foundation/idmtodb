INSERT INTO user_group_idm (iduserExt, idgroupExt, dateFrom) SELECT iduser_idm, idgroup, creation_date FROM user_idm INNER JOIN idmdb_wip.group ON user_idm.division=idmdb_wip.group.name;

INSERT INTO user_group_idm (iduserExt, idgroupExt, dateFrom, type) SELECT iduser_idm, idgroup, creation_date, 0 FROM user_idm INNER JOIN idmdb_wip.group ON user_idm.division=idmdb_wip.group.name;

INSERT INTO user_group_idm (iduserExt, idgroupExt, dateFrom, type) SELECT iduser_idm, idgroup, creation_date, 1 FROM user_idm INNER JOIN idmdb_wip.group ON user_idm.group_name=idmdb_wip.group.name;


cp
opa-dev
cmip
ada
opa-op
sim

[user_groups]
SELECT user_idm.name, user_idm.surname, idmdb_wip.group.name FROM user_idm INNER JOIN user_group_idm ON user_idm.iduser_idm=user_group_idm.iduserExt INNER JOIN idmdb_wip.group ON user_group_idm.idgroupExt=idmdb_wip.group.idgroup;


132 user_idm;
261 user_groups;

3 utenti persi ?? 

DROP VIEW IF EXISTS tmp_user_group; CREATE VIEW tmp_user_group AS (SELECT user_idm.*, idmdb_wip.group.name AS real_group_name, user_group_idm.type FROM user_idm INNER JOIN user_group_idm ON user_idm.iduser_idm=user_group_idm.iduserExt INNER JOIN idmdb_wip.group ON user_group_idm.idgroupExt=idmdb_wip.group.idgroup WHERE user_group_idm.dateTo IS NULL);

SHOW FULL TABLES IN idmdb_wip WHERE TABLE_TYPE LIKE 'VIEW';

select * from tmp_user_group tmp_ug1 INNER JOIN tmp_user_group tmp_ug2 ON tmp_ug1.iduser_idm=tmp_ug2.iduser_idm AND tmp_ug1.type=0 AND tmp_ug2.type=1;


DROP VIEW IF EXISTS user_gd_idm;
CREATE VIEW user_gd_idm AS (select tmp_ug1.iduser_idm, tmp_ug1.username, tmp_ug1.name, tmp_ug1.surname, tmp_ug1.uid, tmp_ug1.gid, tmp_ug2.real_group_name AS group_name, tmp_ug1.real_group_name AS division, tmp_ug1.creation_date, tmp_ug1.expiration_date, tmp_ug1.vpn_expiration_date, tmp_ug1.email, tmp_ug1.notes, tmp_ug1.closing_date, ((tmp_ug2.real_group_name = 'juno-ext')) AS no_cmcc, ((tmp_ug1.closing_date is not null)) AS status from tmp_user_group tmp_ug1 INNER JOIN tmp_user_group tmp_ug2 ON tmp_ug1.iduser_idm=tmp_ug2.iduser_idm AND tmp_ug1.type=0 AND tmp_ug2.type=1);



# per import da MariaDB verso MySQL
ALTER TABLE user_idm DROP COLUMN status;
ALTER TABLE user_idm ADD COLUMN status BOOLEAN GENERATED ALWAYS AS ((`closing_date` is not null)) VIRTUAL;


[BACKUP]
UPDATE user_group_idm AS ug_idm_dst, (SELECT iduser_group_idm, idgroupExt FROM user_group_idm INNER JOIN idmdb_wip.group ON idgroupExt=idgroup WHERE iduserExt = 307 AND dateTo IS NOT NULL) AS ug_idm_src
SET
	ug_idm_dst.dateTo = NULL
WHERE ug_idm_dst.iduser_group_idm=ug_idm_src.iduser_group_idm;

[TEST]
SELECT user_group_idm.* FROM user_group_idm INNER JOIN idmdb_wip.group ON idgroupExt=idgroup WHERE iduserExt = 307; 


[TEST AFTER CHANGING DIVISION]
SELECT user_group_idm.* FROM user_group_idm INNER JOIN idmdb_wip.group ON idgroupExt=idgroup INNER JOIN idmdb_wip.user_idm ON iduser_idm=iduserExt AND surname = 'Chiarelli';

