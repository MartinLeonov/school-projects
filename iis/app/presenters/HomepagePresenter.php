<?php

namespace App\Presenters;

use Nette,
	App\Model;


/**
 * Homepage presenter.
 */
class HomepagePresenter extends BasePresenter
{
    const
        RESERV_TABLE    = 'REZERVACE',
        STAY_TABLE      = 'POBYT',
        STAY_ROOM_TABLE = 'POBYT_POKOJ';

    /** $var Nette\Database\Context */
    private $database;

    public function __construct(Nette\Database\Context $database)
    {
        $this->database = $database;
    }

	public function renderDefault()
	{
        // Get data about free rooms
        $now = strtotime(date("1970-m-d"));

        $types = $this->database->query('SELECT TYP, CENA_DEN, ID,
            UNIX_TIMESTAMP(DATUM_OD) as OD, UNIX_TIMESTAMP(DATUM_DO) as DO
            FROM TYP_POKOJE AS A, CENA AS B
            WHERE A.ID = B.TYP_ID
            ORDER BY A.KAPACITA ASC;');
        $this->template->types = array();

        // Filter only rooms with price in now time
        $endYear = 366 * 24 * 60 * 60;
        foreach ($types->fetchAll() as $row)
        {
            if($row->OD >= $row->DO)
            {
                if($now > 0 and $now <= $row->OD or
                   $now > $row->DO and $now <= $endYear)
                   array_push($this->template->types, $row);
            }
            elseif ($now > $row->OD and $now <= $row->DO)
                array_push($this->template->types, $row);
        }
        $ids = $this->database->query('SELECT B.ID_POBYT_POKOJ
            FROM POBYT AS A, POBYT_POKOJ AS B
            WHERE A.ID_POBYT = B.POBYT AND
            CURDATE() BETWEEN A.DATUM_OD AND A.DATUM_DO;')->fetchAll();

        $array = array();
        foreach ($ids as $id)
            array_push($array, $id->ID_POBYT_POKOJ);
        $free_rooms = $this->database->query('SELECT A.TYP_ID, COUNT(A.CISLO) AS POCET
            FROM POKOJ AS A, POBYT_POKOJ AS B
            WHERE A.CISLO != B.POKOJ AND B.ID_POBYT_POKOJ NOT IN (?)
            GROUP BY A.TYP_ID;', array_values($array))->fetchAll();

        //var_dump($free_rooms);

        foreach ($this->template->types as $type)
        {

            // default value
            $type->COUNT_FREE = 0;
            foreach ($free_rooms as $room)
            {
                if($room->TYP_ID == $type->ID)
                {
                    $type->COUNT_FREE = $room->POCET;
                    continue;
                }
            }
        }
	}

    /**
     * Create form for reservations
     * @return Nette\Application\UI\Form
     */

    public function createComponentReservationsForm()
    {
        $form = new Nette\Application\UI\Form;
        $form->setMethod('GET');

        // zakladni cast formulare
        $form->addText('inputDateBegin', 'Doba rezervace (od)')
             ->setType('date')
             ->setAttribute('placeholder', 'YYYY-MM-DD');
        $form->addText('inputDateEnd', 'Doba rezervace (do)')
             ->setType('date')
             ->setAttribute('placeholder', 'YYYY-MM-DD');

        // prihlaseny uzivatel
        if ($this->getUser()->isLoggedIn()) {
            // prvni mezikrok - zadani data rezervace
            if (!isset($this->params['inputDateBegin']) ||
                !isset($this->params['inputDateEnd'])) {
                $form->addSubmit('send', 'Pokracovat')
                     ->setAttribute('class', 'btn btn-primary');
            // druhy mezikrok - vyhledani volnych pokoju podle zadaneho data
            } else {
                $rooms = $this->database->query("
                    SELECT CISLO, TYP, KAPACITA
                    FROM POKOJ
                      INNER JOIN TYP_POKOJE ON (POKOJ.TYP_ID = TYP_POKOJE.ID)
                    WHERE CISLO IN (
                      SELECT POKOJ
                      FROM POBYT_POKOJ
                        INNER JOIN POBYT ON
                        (POBYT_POKOJ.ID_POBYT_POKOJ = POBYT.ID_POBYT)
                      WHERE
                      ('{$this->params['inputDateBegin']}'
                        NOT BETWEEN POBYT.DATUM_OD AND POBYT.DATUM_DO)
                      AND ('{$this->params['inputDateEnd']}'
                        NOT BETWEEN POBYT.DATUM_OD AND POBYT.DATUM_DO)
                    );"
                )->fetchAll();
                // nacteni nalezenych pokoju do pole
                $empty_rooms = array();
                foreach ($rooms as $room) {
                    $empty_rooms[$room['CISLO']] = "{$room['CISLO']} ({$room['TYP']})";
                }
                // nedostatek pokoju pro pozadavky
                if (count($empty_rooms) == 0) {
                    $this->flashMessage('Pro tento termin nemame zadny volny pokoj. Zkuste si vybrat jiny termin','alert alert-failure');
                    $form->addSubmit('send', 'Pokracovat')
                         ->setAttribute('class', 'btn btn-primary');
                // vybrani pokoju
                } else {
                    $form->addCheckboxList('rooms', 'Volne pokoje', $empty_rooms);
                    $form->addSubmit('send', 'Rezervovat')
                         ->setAttribute('class', 'btn btn-primary');
                }
            }

        // neprihlaseny uzivatel
        } else {
            $form->addSubmit('send', 'Pokracovat')
                 ->setAttribute('class', 'btn btn-primary disabled');
        }

        // odeslani formulare pokud jsou vyplnene vsechny potrebne polozky
        if (isset($this->params['rooms']) && count($this->params['rooms']) != 0) {
            $form->onSuccess[] = $this->createReservation;
        }
        return $form;
    }


    public function createReservation($form, $values)
    {
        // zapis do tabulky REZERVACE
        $row_resv = $this->database->table(self::RESERV_TABLE)->insert(array(
            'DATUM'    => date('Y-m-d'),
            'VLASTNIK' => $this->user->id,
        ));
	$tmpArray = $row_resv->toArray();
        // zapis do tabulky POBYT
        $row_stay = $this->database->table(self::STAY_TABLE)->insert(array(
            'DATUM_OD'      => $this->params['inputDateBegin'],
            'DATUM_DO'      => $this->params['inputDateEnd'],
            'SLEVA'         => null,
            'PLATBA'        => null,
            'PREVZAL_RECEP' => null,
            'ID_REZ'        => $rowArray['ID_REZ'],
            'VLASTNIK'      => $this->user->id,
        ));
	$tmpArray = $row_stay->toArray();
        // zapis do tabulky POBYT_POKOJ
        foreach ($values['rooms'] as $room) {
            $row = $this->database->table(self::STAY_ROOM_TABLE)->insert(array(
                'POBYT' => $tmpArray['ID_POBYT'],
                'POKOJ' => $room,
                'LIDI'  => 0,
            ));
        }
        $this->flashMessage('Rezervace byla vytvorena','alert alert-success');
        $this->redirect('Homepage:reservation');
    }

    public function renderServices()
    {
        if(isset($this->params['id']))
            $this->template->id = $this->params['id'];
        else
            $this->template->id = -1;

        $this->template->services = $this->database->table('SLUZBA');
        $this->template->pobyt = FALSE;
        if($this->user->isInRole('client'))
        {
            $active = $this->database->query("
                        SELECT ID_POBYT
                        FROM POBYT
                          WHERE
                            VLASTNIK = {$this->user->id}
                          AND
                          (NOW()
                            BETWEEN POBYT.DATUM_OD AND POBYT.DATUM_DO);"
                    )->fetchAll();
            $this->template->pobyt = count($active) > 0 ? TRUE : FALSE; 
        } else if($this->user->isLoggedIn)
            $this->template->pobyt = TRUE;
         
    }

    public function handleOrderService($id, $service)
    {
        $result = FALSE;
        
        if($id == -1 && $this->user->isInRole('client'))
        {
            $active = $this->database->query("
                        SELECT ID_POBYT
                        FROM POBYT
                          WHERE
                            VLASTNIK = {$this->user->id}
                          AND
                          (NOW()
                            BETWEEN POBYT.DATUM_OD AND POBYT.DATUM_DO);"
                    )->fetchAll();
            if (count($active) > 0)
            {
                $this->flashMessage('Vyberte k jakemu pokoji sluzba patri', 'alert alert-info');
                $this->redirect('Client:pobytInfo', $active[0]->ID_POBYT, $service); 
            }

        }
 
        // Set client to model
        if ($this->user->isLoggedIn and $id > 0)
        {
            $pobyt =  $this->database->table('POBYT_POKOJ')->get($id)->ref('POBYT');
            if($this->user->isInRole('client') && $pobyt->VLASTNIK != $this->user->id)
                ;
            else    
                $result = $this->database->table('VYUZITI_SLUZBY')->insert(array(
                    'POBYT_POKOJ' => $id,
                    'SLUZBA' => $service,
                    ));
        }

        if($result)
        {
            $this->flashMessage('Sluzba byla objednana.', 'alert alert-success');
             $this->redirect('Client:pobytInfo', $pobyt->ID_POBYT);
        }
        else
        {
            $this->flashMessage('Sluzbu se nepodarilo pridat, zrejme zadny aktualni pobyt.', 'alert alert-danger');
            $this->redirect('Homepage:services');
        }


    }
}
