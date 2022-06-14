<?php
        /*
        ##################################################
        #                                                #
        #         Malware Coder: Elliot Alderson         #
        #                                                #
        #   Github: https://github.com/ElliotAlderson51  #
        #                                                #
        ##################################################
        */
        
# phpmyadmin - utf8mb4_general_ci

session_start();
class db
{
    public $conn;

    public function __construct(){
        $this->conn = mysqli_connect('localhost', 'root', '', 'rat');
    }
}

?>