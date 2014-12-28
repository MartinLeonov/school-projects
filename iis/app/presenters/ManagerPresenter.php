<?php

namespace App\Presenters;

use Nette,
	App\Model;


/**
 * Sign in/out presenters.
 */
class ManagerPresenter extends BasePresenter
{  
  
    /** $var Nette\Database\Context */
    private $database;
    /**
     * @var \App\Model\UserManager
     * @inject
     */
    public $model;

    private $editStaff;

    public function __construct(Nette\Database\Context $database)
    {
        $this->database = $database;
    }

    public function renderDefault()
    {
        $this->template->clients = $this->database->table('KLIENT');
        $this->template->staff = $this->database->table('RECEPCNI');
    }

    public function renderEditStaff()
    {
       //$this->id = $this->params['id'];
    }

    public function actionEditStaff()
    {
        if(isset($this->params['id']))
        {
            $id = $this->params['id'];
            $this->editStaff = $this->model->getStaff($id);
        }
    }

    public function handleDeleteClient($id)
    {
        if(!$this->getUser()->isInRole('admin') or !$this->getUser()->isInRole('staff')  ) {
            $this->flashMessage('Nemate dostatecne opravneni k odstraneni klienta','alert alert-failure');
            $this->redirect('Manager:');
        }

        try {
            $name = $this->model->deletePerson($id, 'KLIENT'); 
            $this->flashMessage('Klient ' . $name . ' byl odstranen.', 'alert alert-success');
            $this->redirect('Manager:');
        }
        catch (\PDOException $e){ 
            $this->flashMessage('Nepodarilo se odstranit klienta','alert alert-failure');
        }

    }

    public function handleDeleteStaff($id)
    {
        if(!$this->getUser()->isInRole('admin')) {
            $this->flashMessage('Nemate dostatecne opravneni k odstraneni recepcni','alert alert-failure');
            $this->redirect('Manager:');

        }

        try {
            $name = $this->model->deletePerson($id, 'RECEPCNI');
            $this->flashMessage('Recepcni ' . $name . ' byla odstranena.', 'alert alert-success');
		    $this->redirect('Manager:');
        }
        catch (\PDOException $e){ 
            $this->flashMessage('Nepodarilo se odstranit recepcni','alert alert-failure');
        }
    }


    /**
     * Sign-up form factory.
     * @return Nette\Application\UI\Form
     */
    protected function createComponentSignClientUpForm()
    {
        $form = new Nette\Application\UI\Form;
        $form->addText('email')
            ->setRequired('Vlozte prosim e-mailovou adresu.')
            ->addRule(NETTE\Forms\Form::MAX_LENGTH, 'E=mail presahuje povolenou delku %d',64)
            ->addRule(Nette\Forms\Form::EMAIL, 'Musite zadat validni e-mailovou adresu')
            ->setType('email') // Nastaveni typu pro HTML5 - mobilni zarizeni
            ->setAttribute('placeholder', 'E-mailova adresa');

        $form->addText('name')
            ->addRule(NETTE\Forms\Form::MAX_LENGTH, 'Jmeno presahuje povolenou delku %d',32)
            ->setRequired('Vlozte prosim krestni jmeno')
            ->setAttribute('placeholder', 'Krestni jmeno');

        $form->addText('surname')
            ->addRule(NETTE\Forms\Form::MAX_LENGTH, 'Prijmeni presahuje povolenou delku %d',32)
            ->setRequired('Vlozte prosim prijmeni')
            ->setAttribute('placeholder', 'Prijmeni');

        $form->addText('phonenumber')
            ->setRequired('Vlozte prosim telefonni cislo')
            ->setAttribute('placeholder', 'Tel. cislo')
            ->setType('tel')
            ->addRule(Nette\Forms\Form::PATTERN, 'Tohle neni validni tel. cislo',
                ' *(\+[0-9]{3} *)?([0-9]{3} *){3}');


        $form->addSubmit('send', 'Registrovat klienta')
            ->setAttribute('class', 'btn btn-large btn-primary');

        $form->onSuccess[] = $this->signClientUpFormSucceeded;
        return $form;
    }


    /**
     * Sign-up form factory.
     * @return Nette\Application\UI\Form
     */
    protected function createComponentSignStaffUpForm()
    {
        $form = new Nette\Application\UI\Form;

        $form->addText('name')
            ->addRule(NETTE\Forms\Form::MAX_LENGTH, 'Jmeno presahuje povolenou delku %d',32)
            ->setRequired('Vlozte prosim krestni jmeno recepcni')
            ->setAttribute('placeholder', 'Krestni jmeno');

        $form->addText('surname')
            ->addRule(NETTE\Forms\Form::MAX_LENGTH, 'Prijmeni presahuje povolenou delku %d',32)
            ->setRequired('Vlozte prosim prijmeni recepcni')
            ->setAttribute('placeholder', 'Prijmeni');

        $form->addText('address')
            ->setRequired('Vlozte prosim adresu recepcni')
            ->setAttribute('placeholder', 'Adresa');

        $form->addPassword('password')
            ->setRequired('Vlozte prosim heslo pro recepcni.')
            ->setAttribute('placeholder', 'Heslo')
            ->addRule(Nette\Forms\Form::MIN_LENGTH, 'Heslo musi obsahovat aspon %d znaky', 4);

        $form->addSubmit('send', 'Registrovat recepcni')
            ->setAttribute('class', 'btn btn-large btn-primary');

        $form->onSuccess[] = $this->signStaffUpFormSucceeded;
        return $form;
    }

    /**
     * Sign-up form factory.
     * @return Nette\Application\UI\Form
     */
    protected function createComponentSignUpForm()
    {
        $form = new Nette\Application\UI\Form;
       	$form->addText('email')
			->setRequired('Vlozte prosim vasi e-mailovou adresu.')
            ->addRule(NETTE\Forms\Form::MAX_LENGTH, 'E=mail presahuje povolenou delku %d',64)
            ->addRule(Nette\Forms\Form::EMAIL, 'Musite zadat validni e-mailovou adresu')
            ->setType('email') // Nastaveni typu pro HTML5 - mobilni zarizeni
            ->setAttribute('placeholder', 'E-mailova adresa');

        $form->addText('name')
            ->addRule(NETTE\Forms\Form::MAX_LENGTH, 'Jmeno presahuje povolenou delku %d',32)
            ->setRequired('Vlozte prosim vase krestni jmeno')
            ->setAttribute('placeholder', 'Krestni jmeno');

        $form->addText('surname')
            ->addRule(NETTE\Forms\Form::MAX_LENGTH, 'Prijmeni presahuje povolenou delku %d',32)
            ->setRequired('Vlozte prosim vase prijmeni')
            ->setAttribute('placeholder', 'Prijmeni');

        $form->addText('phonenumber')
            ->setRequired('Vlozte prosim vase telefonni cislo')
            ->setAttribute('placeholder', 'Tel. cislo')
            ->setType('tel')
            ->addRule(Nette\Forms\Form::PATTERN, 'Tohle neni validni tel. cislo',
                ' *(\+[0-9]{3} *)?([0-9]{3} *){3}');

    
        $form->addPassword('password1')
			->setRequired('Vlozte prosim vase heslo.')
            ->setAttribute('placeholder', 'Heslo')
            ->addRule(Nette\Forms\Form::MIN_LENGTH, 'Heslo musi obsahovat aspon %d znaky', 4);

        $form->addPassword('password2')
			->setRequired('Vlozite prosim znovu vase heslo.')
            ->setAttribute('placeholder', 'Znovu heslo')
            ->addRule(Nette\Forms\Form::EQUAL, 'Hesla se neshoduji', $form['password1']);

        $form->addSubmit('send', 'Registrovat se')
            ->setAttribute('class', 'btn btn-large btn-primary');

        $form->setDefaults($editStaff);
        $form->onSuccess[] = $this->signUpFormSucceeded;
        return $form;
    }

    /**
     * Edit staff form factory.
     * @return Nette\Application\UI\Form
     */
    protected function createComponentEditStaffForm()
    {
        $form = new Nette\Application\UI\Form;

        $form->addHidden('id');
        
        $form->addText('UZIV_JMENO')
            ->addRule(NETTE\Forms\Form::MAX_LENGTH, 'Jmeno presahuje povolenou delku %d',32)
            ->setRequired('Vlozte prosim uzivatelske jmeno')
            ->setAttribute('placeholder', 'Uzivatelske jmeno');

        $form->addText('JMENO')
            ->addRule(NETTE\Forms\Form::MAX_LENGTH, 'Jmeno presahuje povolenou delku %d',32)
            ->setRequired('Vlozte prosim krestni jmeno')
            ->setAttribute('placeholder', 'Krestni jmeno');

        $form->addText('PRIJMENI')
            ->addRule(NETTE\Forms\Form::MAX_LENGTH, 'Prijmeni presahuje povolenou delku %d',32)
            ->setRequired('Vlozte prosim prijmeni')
            ->setAttribute('placeholder', 'Prijmeni');

        $form->addText('ADRESA')
            ->setRequired('Vlozte prosim adresu recepcni')
            ->setAttribute('placeholder', 'Adresa');

        $form->addSubmit('send', 'Editovat udaje')
            ->setAttribute('class', 'btn btn-primary');

        if(isset($this->editStaff))
        {
            $form->setDefaults($this->editStaff);
            $form['id']->setValue($this->editStaff['OS_CISLO']);
        }


        $form->onSuccess[] = $this->editStaffFormSucceeded;

        return $form;
    }

    /**
     * Edit staff form factory.
     * @return Nette\Application\UI\Form
     */
    protected function createComponentPasswordStaffForm()
    {
        $form = new Nette\Application\UI\Form;
        $form->addHidden('id')->setValue($this->editStaff['OS_CISLO']);

        $form->addPassword('password1')
            ->setRequired('Vlozte prosim heslo.')
            ->setAttribute('placeholder', 'Heslo')
            ->addRule(Nette\Forms\Form::MIN_LENGTH, 'Heslo musi obsahovat aspon %d znaky', 4);

        $form->addPassword('password2')
            ->setRequired('Vlozite prosim znovu heslo.')
            ->setAttribute('placeholder', 'Znovu heslo')
            ->addRule(Nette\Forms\Form::EQUAL, 'Hesla se neshoduji', $form['password1']);

        $form->addSubmit('send', 'Zmenit heslo')
            ->setAttribute('class', 'btn btn-primary');


        $form->onSuccess[] = $this->passwordStaffFormSucceeded;

        return $form;
    }

	public function signUpFormSucceeded($form, $values)
	{
        if ($this->getUser()->isLoggedIn()){
            $form->addError('Nelze se registrovat z prihlaseneho uzivatele');
            return;
        }
		if ($values->password1 !== $values->password2) {
            $form->addError('Hesla se neshoduji');
            return;
		}

        try {
            $container = require __DIR__ . '/../../app/bootstrap.php';
            $container->getByType('App\Model\UserManager')->addClient($values->email, $values->password1, $values->name,
                $values->surname, $values->phonenumber);
			$this->flashMessage('Byl jste uspesne zaregistrovan. Muzete se prihlasit.', 'alert alert-success');
			$this->redirect('Sign:in');
		} catch (\PDOException $e) {
			$form->addError('Nepodarilo se vytvorit novy ucet. Nejspise uz jeden se stejnym emailem existuje');
		}
	}

    public function signClientUpFormSucceeded($form, $values)
    {
        try {
            $this->model->addClientByStaff($values->email, $values->name,
                $values->surname, $values->phonenumber);
            $this->flashMessage('Klient byl uspesne zaregistrovan.', 'alert alert-success');
            $this->redirect('Manager:default');
        } catch (\PDOException $e) {
            $form->addError('Nepodarilo se vytvorit novy ucet. Nejspise uz jeden se stejnym emailem existuje');
        }
    }

    public function signStaffUpFormSucceeded($form, $values)
    {
        try {
            $username = ucfirst(trim($values->name)) . ucfirst(trim($values->surname));
            $this->model->addStaff($username, $values->password, $values->name,
                $values->surname, $values->address);
            $this->flashMessage('Recepcni byla uspesne zaregistrovana. Uzivatelske jmeno je: ' . $username, 'alert alert-success');
            $this->redirect('Manager:default');
        } catch (\PDOException $e) {
            $form->addError('Nepodarilo se vytvorit novy ucet. Nejspise uz jeden se stejnym jmenem a prijmenim existuje.');
        }
    }

    public function editStaffFormSucceeded($form, $values)
    {

        try {
            $this->model->editStaff($values->id, $values->UZIV_JMENO, $values->JMENO,
                $values->PRIJMENI, $values->ADRESA);
            $this->flashMessage('Udaje recepcni '. $values->PRIJMENI .' byly uspesne upraveny' , 'alert alert-success');
            $this->redirect('Manager:default');
        } catch (\PDOException $e) {
            $form->addError('Nepodarilo se upravit ucet recepcni. Nekde nastala chyba.');
        }
    }

    public function passwordStaffFormSucceeded($form, $values)
    {
        if ($values->password1 !== $values->password2) {
            $form->addError('Hesla se neshoduji');
            return;
        }

        try {
            $this->model->editStaffPassword($values->id, $values->password1);
            $this->flashMessage('Heslo recepcni bylo uspesne zmeneno' , 'alert alert-success');
            $this->redirect('Manager:default');
        } catch (\PDOException $e) {
            $form->addError('Nepodarilo se zmenit heslo recepcni. Nekde nastala chyba.');
        }
    }




}
