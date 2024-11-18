grammar CGrammar;
prog: primaryExp+ EOF
primaryExp: multDivExp;
multDivExp: primaryExp (('*'|'/'|'%') primaryExp)*;
addSubExp: multDivExp (('+'|'-') multDivExp)*;