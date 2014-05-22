--TRIGGER. Pokud smazeme typ pokoje, musi se vsechny tyhle pokoje smazat
CREATE OR REPLACE TRIGGER smazat_pokoje 
BEFORE DELETE ON TYP_POKOJE
BEGIN
	DELETE FROM POKOJ
	WHERE POKOJ.TYP = TYP;
END smazat_pokoje;
