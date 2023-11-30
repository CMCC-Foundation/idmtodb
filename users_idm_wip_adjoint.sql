CREATE TABLE IF NOT EXISTS `mydb`.`group` (
  `idgroup` INT NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(45) NOT NULL,
  `desc` VARCHAR(512) NULL,
  `status` TINYINT UNSIGNED NULL DEFAULT 1,
  PRIMARY KEY (`idgroup`))
ENGINE = InnoDB

CREATE TABLE IF NOT EXISTS `mydb`.`user_group_idm` (
  `iduser_group_idm` INT NOT NULL,
  `iduserExt` INT NOT NULL,
  `idgroupExt` INT NOT NULL,
  `dateFrom` DATE NULL,
  `dateTo` DATE NULL,
  PRIMARY KEY (`iduser_group_idm`),
  INDEX `fk_user_group_idm_1_idx` (`iduserExt` ASC) VISIBLE,
  INDEX `fk_user_group_idm_2_idx` (`idgroupExt` ASC) VISIBLE,
  CONSTRAINT `fk_user_group_idm_1`
    FOREIGN KEY (`iduserExt`)
    REFERENCES `mydb`.`user_idm` (`iduser_idm`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_user_group_idm_2`
    FOREIGN KEY (`idgroupExt`)
    REFERENCES `mydb`.`group` (`idgroup`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB
