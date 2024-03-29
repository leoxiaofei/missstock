CREATE TABLE ProductInfo (
pi_ID   INTEGER  PRIMARY KEY AUTOINCREMENT,
pi_Name TEXT,
pi_Desc TEXT
);

CREATE TABLE ProductDetailInfo (
pdi_Serial   TEXT  PRIMARY KEY,
pi_ID        INTEGER,
pdi_Type     INTEGER,
pdi_Desc     TEXT
);

CREATE TABLE LoanDetail (
ld_ID       INTEGER  PRIMARY KEY AUTOINCREMENT,
pdi_Serial  TEXT,
ld_LoanDate TEXT,
ei_ID       INTEGER,
fi_ID       INTEGER,
ld_PlanDate TEXT,
ld_State    INTEGER
);

/*
CREATE TABLE LoanEx (
le_ID      INTEGER  PRIMARY KEY,
ld_ID      TEXT,
pdi_Serial TEXT
);
*/

CREATE TABLE EmployeeInfo (
ei_ID   INTEGER  PRIMARY KEY AUTOINCREMENT,
ei_Name TEXT
);

CREATE TABLE FinalUserInfo (
fi_ID   INTEGER  PRIMARY KEY AUTOINCREMENT,
fi_Name TEXT
);

CREATE TABLE PutBook (
pb_ID      INTEGER  PRIMARY KEY,
pdi_Serial TEXT,
pb_PutDate TEXT,
pb_Reason  INTEGER
);

CREATE TABLE OutBook (
ob_ID      INTEGER  PRIMARY KEY,
pdi_Serial TEXT,
ob_PutDate TEXT,
ob_Reason  INTEGER
);

CREATE TABLE CurrentStock (
cs_ID      INTEGER  PRIMARY KEY,
pdi_Serial TEXT,
cs_Shelf   TEXT
);

CREATE TABLE UserInfo (
ui_ID       INTEGER  PRIMARY KEY,
ui_User     TEXT,
ui_Password TEXT,
ui_Right    TEXT
);

CREATE TABLE LoginLog (
ll_ID       INTEGER  PRIMARY KEY,
ui_ID       TEXT,
ll_Date     TEXT
);