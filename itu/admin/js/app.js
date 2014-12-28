(function(){
    var app = angular.module('email', []);
    app.controller('EmailController', function($scope){
	$scope.mailbox = {};
        $scope.mailbox.doClick = function(address, type)
        {
            $scope.mailbox.address = address;
            $scope.mailbox.type = type;
            $scope.mailbox.edit = true;

            var $radios = $('input:radio[name=editType]');

            $radios.filter('[value!='+ type +']').prop('checked', false).parent().removeClass('active');
            $radios.filter('[value='+ type +']').prop('checked', true).parent().addClass('active');

            setTimeout(function(){window.scrollTo(0,document.body.scrollHeight);}, 10);
        }
    });
})();

