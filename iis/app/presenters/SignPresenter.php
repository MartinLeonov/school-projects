<?php

namespace App\Presenters;

use Nette,
	App\Model;


/**
 * Sign in/out presenters.
 */
class SignPresenter extends BasePresenter
{

     /**
     * @var \App\Model\UserManager
     * @inject
     */
    public $model;
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

        $form->onSuccess[] = $this->signUpFormSucceeded;
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
            $this->model->addClient($values->email, $values->password1, $values->name,
                $values->surname, $values->phonenumber);
			$this->flashMessage('Byl jste uspesne zaregistrovan. Muzete se prihlasit.', 'alert alert-success');
			$this->redirect('Sign:in');
		} catch (\PDOException $e) {
			$form->addError('Nepodarilo se vytvorit novy ucet. Nejspise uz jeden se stejnym emailem existuje');
		}
	}


	/**
	 * Sign-in form factory.
	 * @return Nette\Application\UI\Form
	 */
	protected function createComponentSignInForm()
	{
		$form = new Nette\Application\UI\Form;
		$form->addText('email')
			->setRequired('Vlozte prosim vasi e-mailovou adresu.')
            ->setType('email') // Nastaveni typu pro HTML5 - mobilni zarizeni
            ->setAttribute('placeholder', 'E-mailova adresa');
     
        $form->addHidden('back')
            ->setValue(isset($this->params['back']) ? $this->params['back'] : NULL);


		$form->addPassword('password')
			->setRequired('Vlozte prosim vase heslo.')
            ->setAttribute('placeholder', 'Heslo');

		$form->addCheckbox('remember', 'Trvale prihlaseni')
            ->setAttribute('valign','top');



		$form->addSubmit('send', 'Prihlasit se')
            ->setAttribute('class', 'btn btn-large btn-primary');

		// call method signInFormSucceeded() on success
		$form->onSuccess[] = $this->signInFormSucceeded;
		return $form;
	}


	public function signInFormSucceeded($form, $values)
	{
		if ($values->remember) {
			$this->getUser()->setExpiration('14 days', FALSE);
		} else {
			$this->getUser()->setExpiration('20 minutes', TRUE);
		}

		try {
			$this->getUser()->login($values->email, $values->password);
			$this->flashMessage('Byl jste prihlasen.', 'alert alert-success');

            if($values->back != NULL)
                $this->redirect($values->back);
            else if ($this->getUser()->isInRole('client'))
			    $this->redirect('Homepage:');
            else
                $this->redirect('Manager:');
		} catch (Nette\Security\AuthenticationException $e) {
			$form->addError($e->getMessage());
		}
	}


	public function actionOut()
	{
		$this->getUser()->logout();
		$this->flashMessage('Byl jste odhlasen.', 'alert alert-success');
		$this->redirect('Homepage:');
	}

}
