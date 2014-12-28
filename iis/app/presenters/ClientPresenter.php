<?php

namespace App\Presenters;

use Nette,
	App\Model;


/**
 * Sign in/out presenters.
 */
class ClientPresenter extends BasePresenter
{  
  
    /** $var Nette\Database\Context */
    private $database;
    /**
     * @var \App\Model\Client
     * @inject
     */
    public $model;

    public function __construct(Nette\Database\Context $database)
    {
        $this->database = $database;
    }



    public function renderDefault($id = NULL)
    {
        if(isset($this->params['id']))
            $id = $this->params['id'];
        if($id)
        {
            $client = $this->model->getClient($this->params['id']);
            if($client != NULL)
            {
                $time = time();
                $this->template->client = $client;
                $this->template->auth = $this->model->hasAuth();
                $this->template->rezervations = $this->model->getRezervations();
                $this->template->pobyty = $this->model->getOnlyPobyty();
                
                $this->template->isClient = FALSE;
                if ($client->OS_CISLO == $this->user->id)
                    $this->template->isClient = TRUE;

                foreach($this->template->rezervations as &$rez)
                {
                    $rez['DISABLED'] = FALSE;

                    foreach($rez['POBYTY'] as $pobyt)
                    {
                        if($pobyt->DATUM_OD->getTimestamp() < $time)
                            $rez['DISABLED'] = TRUE;
                        
                        if(($pobyt['DATUM_OD']->getTimestamp() < $time) &&
                           ($pobyt['DATUM_DO']->getTimestamp() > $time))
                            $this->template->aktualni = $pobyt;
                    }
     
                }

                foreach($this->template->pobyty as &$pobyt)
                {
                    if(($pobyt['DATUM_OD']->getTimestamp() < $time) &&
                       ($pobyt['DATUM_DO']->getTimestamp() > $time))
                        $this->template->aktualni = $pobyt;

                    $pobyt['DISABLED'] = FALSE;
                    if($pobyt['DATUM_OD']->getTimestamp() < $time)
                        $pobyt['DISABLED'] = TRUE;

                }

            }
            
        }
    }


    public function renderAddToRes($id)
    {
        if(!isset($this->params['id_client']))
        {
            $this->flashMessage('Neco spatneho se stalo pri zpracovani.', 'alert alert-danger');
        
            $this->redirect('Manager:');

        }
    }

    public function renderPobytInfo($id, $service = NULL)
    {
        if(!isset($this->params['id']))
        {
            $this->flashMessage('Neco spatneho se stalo pri zpracovani.', 'alert alert-danger');
            $this->redirect('Manager:');
        }

  
        $this->template->service = $service;

        $this->template->pobyt = $this->database->table('POBYT')->get($id);
        if ($this->template->pobyt->PREVZAL_RECEP != NULL)
        {
            $this->template->prevzal = $this->database->table('RECEPCNI')->get($this->template->pobyt->PREVZAL_RECEP)->PRIJMENI;
        }
        $rooms = $this->template->pobyt->related('POBYT_POKOJ');

        $this->template->rooms = array();
        $this->template->services = array();
        foreach($rooms as $room)
        {
            $type = $this->database->table('TYP_POKOJE')->where('ID', $room->ref('POKOJ')->TYP_ID)->fetch();
            array_push($this->template->rooms, array_merge($room->toArray(), $type->toArray()));


            foreach($room->related('VYUZITI_SLUZBY') as $service)
            {
          
                $ser = $service->toArray();
                $ser['NAZEV'] = $service->ref('SLUZBA')->NAZEV;
                $ser['POKOJ'] = $room->POKOJ;
                array_push($this->template->services, $ser);
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
        if(isset($this->params['id_rez']))
            $form->addHidden('id_rez')
                ->setValue($this->params['id_rez']);

        $form->addHidden('id_client')
            ->setValue($this->params['id_client']);

        // prihlaseny uzivatel
        if ($this->getUser()->isLoggedIn()) {
            // prvni mezikrok
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
                    $form->addCheckboxList('rooms','Volne pokoje',$empty_rooms);
                    $form->addSubmit('send', 'Pridat Pobyt')
                          ->setAttribute('class', 'btn btn-primary');

                }
            }

            if(isset($this->params['rooms']) && count($this->params['rooms']) != 0)
                 $form->onSuccess[] = $this->addToRes;

            return $form;
        } 
        return NULL;
    }

    public function addToRes($form, $values)
    {
        if (count($values->rooms) == 0)
        {
            $this->flashMessage('Musi byt vybrat aspon jeden pokoj.', 'alert alert-danger');
            return;
        }

        $reservation = (isset($values->id_rez)) ? $values->id_rez : NULL;
        try {
            $this->model->addPobyt($values->rooms, $values->inputDateBegin, $values->inputDateEnd, $reservation, $values->id_client);
            $this->flashMessage('Pobyt byl pridan.', 'alert alert-success');

        } catch (\PDOException $e) {
            $this->flashMessage('Nepodarilo se pridat pobyt.', 'alert alert-danger');
        }
        $this->redirect('Client:', $values->id_client);

    }

    public function handleGeneratePassword($id)
    {
        $result = FALSE;
        // Generate auth info
        if ($this->user->loggedIn && !$this->user->isInRole('client') ||
            $this->user->isInRole('client') && $id == $this->user->id)
            $result = $this->model->generateAuthInfo($id);

        if($result)
        {
            $this->flashMessage('Heslo bylo vygenerovano a poslano na e-mail klienta.', 'alert alert-success');
             $this->redirect('Client:default', $id);
        }
        else
        {
            $this->flashMessage('Heslo se nepodarilo vygenerovat.', 'alert alert-danger');
             $this->redirect('Client:default', $id);
        }

    }

    public function handleResetPassword($id)
    {
        $result = FALSE;
        // Generate auth info
        if ($this->user->loggedIn && !$this->user->isInRole('client') ||
            $this->user->isInRole('client') && $id == $this->user->id)
            $result = $this->model->resetPassword($id);

        if($result)
        {
            $this->flashMessage('Heslo bylo resetovano a zaslano na e-mail klienta.', 'alert alert-success');
             $this->redirect('Client:default', $id);
        }
        else
        {
            $this->flashMessage('Heslo se nepodarilo resetovat.', 'alert alert-danger');
             $this->redirect('Client:default', $id);
        }

    }

    public function handleDeletePobyt($pobyt, $id)
    {
        $result = FALSE;
        // Set client to model
        if ($this->user->loggedIn && !$this->user->isInRole('client'))
            $result = $this->model->deletePobyt($pobyt);

        if($this->user->isInRole('client'))
            $result = $this->model->deletePobyt($pobyt, $this->user->id);

        if($result)
        {
            $this->flashMessage('Pobyt byl odstranen.', 'alert alert-success');
             $this->redirect('Client:default', $id);
        }
        else
        {
            $this->flashMessage('Pobyt se nepodarilo odstranit.', 'alert alert-danger');
             $this->redirect('Client:default', $id);
        }

    }

    public function handleDeleteReservation($res, $id)
    {
        $result = FALSE;
        // Set client to model
        if ($this->user->loggedIn && !$this->user->isInRole('client'))
            $result = $this->model->deleteReservation($res);

        if($this->user->isInRole('client'))
            $result = $this->model->deleteReservation($res, $this->user->id);

        if($result)
        {
            $this->flashMessage('Rezervace byla odstranen.', 'alert alert-success');
             $this->redirect('Client:default', $id);
        }
        else
        {
            $this->flashMessage('Rezervaci se nepodarilo odstranit. Nejspise uz nektery z pobyty zacal.', 'alert alert-danger');
             $this->redirect('Client:default', $id);
        }

    }

    public function handleCreateReservation($id)
    {
        $result = FALSE;
        // Set client to model
        if ($this->user->loggedIn && !$this->user->isInRole('client') ||
            $this->user->isInRole('client') && $id == $this->user->id)
            $result = $this->model->createReservation($id);

        if($result)
        {
            $this->flashMessage('Rezervace byla pridana. Muzete pridat pobyty.', 'alert alert-success');
             $this->redirect('Client:default', $id);
        }
        else
        {
            $this->flashMessage('Rezervaci se nepodarilo pridat.', 'alert alert-danger');
             $this->redirect('Client:default', $id);
        }

    }

    public function handleGetBill($id)
    {
        $result = FALSE;
        // Set client to model
        if ($this->user->loggedIn && !$this->user->isInRole('client'))
            $result = $this->model->getBill($id);

        if($result)
        {
            $this->flashMessage('Ucet byl vystaven.', 'alert alert-success');
             $this->redirect('Client:pobytInfo', $id);
        }
        else
        {
            $this->flashMessage('Nepodarilo se vystavit ucet. Pravdepodobne je ucet zaplacen.', 'alert alert-danger');
             $this->redirect('Client:default', $id);
        }

    }

    public function handleGetPayment($id)
    {
        $result = FALSE;
        // Set client to model
        if ($this->user->loggedIn && $this->user->isInRole('staff'))
        {
            $row = $this->database->table('POBYT')->get($id);
            if($row->PLATBA != 0)
                $result = $row->update(array(
                                'PREVZAL_RECEP' => $this->user->id,
                            ));
        }

        if($result)
        {
            $this->flashMessage('Castka byla prevzata.', 'alert alert-success');
             $this->redirect('Client:pobytInfo', $id);
        }
        else
        {
            $this->flashMessage('Castku se nepodarilo zapsat jako prevzatou.', 'alert alert-danger');
             $this->redirect('Client:pobytInfo', $id);
        }

    }

     public function handleOrderService($id, $service)
    {
        $result = FALSE;
        
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
            $this->redirect('Client:pobytInfo', $pobyt->ID_POBYT);
        }


    }


}
