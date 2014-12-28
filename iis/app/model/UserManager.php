<?php

namespace App\Model;

use Nette,
	Nette\Utils\Strings,
	Nette\Security\Passwords;


/**
 * Users management.
 */
class UserManager extends Nette\Object implements Nette\Security\IAuthenticator
{
    const
        AUTH_TABLE = 'AUTENTIFIKACE',

        STAFF_TABLE = 'RECEPCNI',
		CLIENT_TABLE = 'KLIENT',
        
        COLUMN_USERNAME = 'UZIV_JMENO',
		COLUMN_ID = 'OS_CISLO',
        
        COLUMN_ADDRESS = 'ADRESA',
		COLUMN_NAME = 'JMENO',
		COLUMN_PASSWORD_HASH = 'HESLO',
        COLUMN_SURNAME = 'PRIJMENI',
        COLUMN_EMAIL = 'EMAIL',
        COLUMN_TELNUM = 'TEL_CISLO';


	/** @var Nette\Database\Context */
	private $database;


	public function __construct(Nette\Database\Context $database)
	{
		$this->database = $database;
	}


	/**
	 * Performs an authentication.
	 * @return Nette\Security\Identity
	 * @throws Nette\Security\AuthenticationException
	 */
	public function authenticate(array $credentials)
	{
		list($username, $password) = $credentials;

		$row = $this->database->table(self::AUTH_TABLE)->where(self::COLUMN_USERNAME, $username)->fetch();

		if (!$row) {
			throw new Nette\Security\AuthenticationException('The username is incorrect.', self::IDENTITY_NOT_FOUND);

		} elseif (!Passwords::verify($password, $row[self::COLUMN_PASSWORD_HASH])) {
			throw new Nette\Security\AuthenticationException('The password is incorrect.', self::INVALID_CREDENTIAL);

		} elseif (Passwords::needsRehash($row[self::COLUMN_PASSWORD_HASH])) {
			$row->update(array(
				self::COLUMN_PASSWORD_HASH => Passwords::hash($password),
			));
		}
        $auth = $row->toArray();

        
        $role = 0;
        if ($auth[self::CLIENT_TABLE] != NULL) {
            $row = $row->ref(self::CLIENT_TABLE);
            $role = 'client';
        }
        else if ($auth[self::STAFF_TABLE] != NULL) {
            $row = $row->ref(self::STAFF_TABLE);
            $role = 'staff';
        }
        else // Je to admin
            $role = 'admin';

        // Jestli se nejedna o Admina, pridame nektere informace
        if ($role != 'admin' )
        {
            $arr = $row->toArray();
            $id = $row[self::COLUMN_ID];
        }
        else
        {
            $id = $row[self::COLUMN_USERNAME];
            $arr = NULL;
        }
		return new Nette\Security\Identity($id, $role, $arr);
	}


	/**
	 * Adds new client.
     * @param  string
     * @param  string
     * @param  string
	 * @param  string
	 * @param  string
	 * @return void
	 */
	public function addClient($email, $password, $name, $surname, $telnumber)
	{
		$row = $this->database->table(self::CLIENT_TABLE)->insert(array(
			self::COLUMN_EMAIL => $email,
            self::COLUMN_NAME => $name,
            self::COLUMN_SURNAME => $surname,
            self::COLUMN_TELNUM => $telnumber,
		));
        try {
	    $tmp = $row->toArray();
            $this->database->table(self::AUTH_TABLE)->insert(array(
                self::COLUMN_USERNAME => $email,
                self::COLUMN_PASSWORD_HASH => Passwords::hash($password),
                self::CLIENT_TABLE => $tmp[self::COLUMN_ID],
            ));
        }
        catch (\PDOException $e)
        {
            $this->database->table(self::CLIENT_TABLE)->where(self::COLUMN_ID, $id)->delete();
            throw \PDOException;
        }

	}

        /**
     * Adds new client without authetification.
     * @param  string
     * @param  string
     * @param  string
     * @param  string
     * @param  string
     * @return void
     */
    public function addClientByStaff($email, $name, $surname, $telnumber)
    {
        $row = $this->database->table(self::CLIENT_TABLE)->insert(array(
            self::COLUMN_EMAIL => $email,
            self::COLUMN_NAME => $name,
            self::COLUMN_SURNAME => $surname,
            self::COLUMN_TELNUM => $telnumber,
        ));
    }

   	/**
	 * Adds new staff.
     * @param  string
     * @param  string
     * @param  string
	 * @return void
	 */
	public function addStaff($username, $password, $name, $surname, $address)
	{
		$row = $this->database->table(self::STAFF_TABLE)->insert(array(
            self::COLUMN_NAME => $name,
            self::COLUMN_SURNAME => $surname,
            self::COLUMN_ADDRESS => $address,
		));
        try {
	    $tmp = $row->toArray();
            $this->database->table(self::AUTH_TABLE)->insert(array(
                self::COLUMN_USERNAME => $username,
                self::COLUMN_PASSWORD_HASH => Passwords::hash($password),
                self::STAFF_TABLE => $tmp[self::COLUMN_ID],
            ));
        }
        catch (\PDOException $e)
        {
            $this->database->table(self::STAFF_TABLE)->where(self::COLUMN_ID, $id)->delete();
            throw \PDOException;
        }
	}

    /**
     * Edit staff.
     * @param  int
     * @param  string
     * @param  string
     * @param  string
     * @return void
     */
    public function editStaff($id, $username, $name, $surname, $address)
    {
        $row = $this->database->table(self::STAFF_TABLE)->get($id);
        //$data = $row->fetch();

        $auth = $row->related(self::AUTH_TABLE)->fetch();
        $auth->update(array(
            self::COLUMN_USERNAME => $username,
        ));

 
        $row->update(array(
            self::COLUMN_NAME => $name,
            self::COLUMN_SURNAME => $surname,
            self::COLUMN_ADDRESS => $address,
        ));
    }

    /**
     * Get staff.
     * @param  int
     * @return array
     */
    public function getStaff($id)
    {
        $row = $this->database->table(self::STAFF_TABLE)->get($id);
 
        //var_dump($row);
        $auth = $row->related(self::AUTH_TABLE)->fetch();
        //var_dump($auth);
        return array_merge($row->toArray(), $auth->toArray());
    }

    /**
     * Get staff.
     * @param  int
     * @param  string
     * @return array
     */
    public function editStaffPassword($id, $password)
    {
        $row = $this->database->table(self::AUTH_TABLE)->where('RECEPCNI', $id);
        $row->update(array(self::COLUMN_PASSWORD_HASH => Passwords::hash($password)));

        return;
    }

    
    public function deletePerson($id, $table)
    {
        // Kompletni smazani vsechn informaci o klientovi nebo recepcni
        $row = $this->database->table($table)->where('OS_CISLO', $id);
        $data = $row->fetch();
        
        if($table == 'KLIENT')
        {
            $rezervations = array();
            foreach ($data->related('POBYT') as $pobyt)
            {
                $rooms = $pobyt->related('POBYT_POKOJ');

                $rez = $pobyt->ref('ID_REZ');
                if ($rez != NULL) 
                {
                    $pobyt->update(array('ID_REZ' => NULL));
                    array_push($rezervations, $rez);
                }


                foreach($rooms as $room)
                {
                    foreach($room->related('VYUZITI_SLUZBY') as $service)
                        $service->delete();
                    $room->delete();
                }

                $pobyt->delete();

            }
            // Nakonec smazeme vsechny rezervace
            foreach ($rezervations as $rezervation)
                $rezervation->delete();
        }
        else // Jedna se o recepcni -> smazeme prevzate platby
        {
            foreach ($data->related('POBYT') as $pobyt)
                $pobyt->update(array('PREVZAL'=> NULL));
        }

        $this->database->table('AUTENTIFIKACE')->where($table, $data['OS_CISLO'])->delete();

        $row->delete();
        return $data['PRIJMENI'];
    }

}
