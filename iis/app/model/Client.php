<?php

namespace App\Model;

use Nette,
	Nette\Utils\Strings,
    Nette\Security\Passwords;

/**
 * Client management.
 */
class Client extends Nette\Object
{
    const
        AUTH_TABLE = 'AUTENTIFIKACE',
		CLIENT_TABLE = 'KLIENT',
        REZ_TABLE = 'REZERVACE',
        POB_TABLE = 'POBYT';


	/** @var Nette\Database\Context */
	private $database;

    private $client;
    private $rezervations;
    private $pobyty;

	public function __construct(Nette\Database\Context $database)
	{
		$this->database = $database;
	}

    /**
     * Get and set client.
     * @param  int
     * @return array
     */
    public function getClient($id)
    {
        $this->client = $this->database->table(self::CLIENT_TABLE)->get($id);
        return $this->client;
    }

    /**
     * Return true if the client has auth info
     * @return bool
     */
    public function hasAuth()
    {
        if($this->client == NULL)
            return FALSE;

        if(count($this->client->related(self::AUTH_TABLE)) == 1)
            return TRUE;

        return FALSE;
    }


    /**
     * Get rezervations.
     * @param  int
     * @return array
     */
    public function getRezervations()
    {
        $this->rezervations = $this->client->related(self::REZ_TABLE);

        $tmp = array();
        foreach($this->rezervations as $rez)
        {
            $rezervace = $rez->toArray();
            $rezervace['POBYTY'] = $rez->related(self::POB_TABLE);
            array_push($tmp, $rezervace);
        }

        return $tmp;
    }

    /**
     * Get pobyty without rezervations.
     * @return array
     */
    public function getOnlyPobyty()
    {
        $this->pobyty = $this->client->related(self::POB_TABLE);

        $tmp = array();
        foreach($this->pobyty as $pob)
        {
            $pobyt = $pob->toArray();
            if($pobyt['ID_REZ'] == NULL)
                array_push($tmp, $pobyt);
        }

        return $tmp;
    }


    /**
     * Delete one pobyt
     * @param  int
     * @param  int
     * @return array
     */
    public function deletePobyt($id, $client_id = NULL)
    {
        $pobyt = $this->database->table(self::POB_TABLE)->get($id);

        if($client_id != NULL and $pobyt->VLASTNIK != $client_id)
            return FALSE;

        if($pobyt->DATUM_DO->getTimestamp() < time())
            return FALSE;

        $pobyt->delete();

        return TRUE;
    }

    /**
     * Delete one pobyt
     * @param  int
     * @param  int
     * @return array
     */
    public function deleteReservation($id, $client_id = NULL)
    {
        $res = $this->database->table(self::REZ_TABLE)->get($id);

        if($client_id != NULL and $res->VLASTNIK != $client_id)
            return FALSE;


        foreach($res->related(self::POB_TABLE) as $pobyt)
        {
            if($pobyt->DATUM_OD->getTimestamp() < time())
                return FALSE;
            foreach($pobyt->related('POBYT_POKOJ') as $pokoj)
            {
                if(count($pokoj->related('VYUZITI_SLUZBY')) != 0)
                    return FALSE;
            }
        }

        foreach($res->related(self::POB_TABLE) as $pobyt)
        {
            foreach($pobyt->related('POBYT_POKOJ') as $pokoj)
                $pokoj->delete();
            $pobyt->delete();
        }

        $res->delete();

        return TRUE;
    }

    /**
     * Delete empty rezervation
     * @param  int
     * @return array
     */
    public function createReservation($id)
    {
        return $this->database->table(self::REZ_TABLE)->insert(array(
            'DATUM' => date('Y-m-d'),
            'VLASTNIK' => $id,
        ));

    }

    /**
     * Add To reservation
     * @param  ...
     * @return bool
     */
    public function addPobyt($rooms, $date_from, $date_to, $id_rez, $id_client)
    {
        if ($id_rez == NULL and $id_client == NULL)
            return FALSE;

        $row = $this->database->table(self::POB_TABLE)->insert(array(
            'DATUM_OD' => $date_from,
            'DATUM_DO' => $date_to,
            'SLEVA' => 0,
            'PLATBA' => 0,
            'PREVZAL_RECEP' => NULL,
            'ID_REZ' => $id_rez,
            'VLASTNIK' => $id_client,
        ));

	$tmp = $row->toArray();
        $id = $tmp['ID_POBYT'];

        foreach($rooms as $room)
        {
            $this->database->table('POBYT_POKOJ')->insert(array(
                'POBYT' => $id,
                'POKOJ' => $room,
                'LIDI' => 0,
                ));

        }

        return TRUE;

    }


    /**
     * Generate auth info
     * @param  int
     * @return bool
     */
    public function generateAuthInfo($id)
    {
        $this->getClient($id);

        if(count($this->client->related(self::AUTH_TABLE)) != 0)
            return FALSE;

        return $this->database->table(self::AUTH_TABLE)->insert(array(
            'UZIV_JMENO' => $this->client->EMAIL,
            'KLIENT' => $id,
            'HESLO' => Passwords::hash('heslo'),
            'RECEPCNI' => NULL,
        ));
    }

    /**
     * Reset password
     * @param  int
     * @return bool
     */
    public function resetPassword($id)
    {
        $this->getClient($id);

        if(count($this->client->related(self::AUTH_TABLE)) != 1)
            return FALSE;

        $row = $this->client->related(self::AUTH_TABLE)->fetch();
        return $row->update(array(
            'HESLO' => Passwords::hash('heslo'),
        ));

    }

    /**
     * Count all the money
     * @param  int
     * @return bool
     */
    public function getBill($id)
    {
        $row = $this->database->table(self::POB_TABLE)->get($id);

        if(!$row)
            return FALSE;

        $days = $row->DATUM_OD->diff($row->DATUM_DO)->days;

        $payment = 0;
        foreach($row->related('POBYT_POKOJ') as $room)
        {
            $cost = $this->database->table('CENA')->where('ID_CENA', $room->ref('POKOJ')->TYP_ID)->fetch();
            $payment += $days * $cost->CENA_DEN;
            foreach($room->related('VYUZITI_SLUZBY') as $service)
            {
                $payment += $service->ref('SLUZBA')->CENA;
            }
        }
        
        return $row->update(array(
            'PLATBA' => $payment,
        ));
    }
}
