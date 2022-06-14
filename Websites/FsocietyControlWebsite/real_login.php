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
    include 'db.php';

   class Login extends db
   {
        public function checkUser($username, $pass)
        {
            $password = md5($pass);
            $sql = "SELECT * FROM users WHERE username='$username' AND password='$password'";
            $result = mysqli_query($this->conn, $sql);
            $nRows = $result->num_rows;

            if($nRows != 0)
            {
                $row = mysqli_fetch_assoc($result);
                $_SESSION['username'] = $row['username'];
                $_SESSION['user_login_control_success'] = "good";
                $_SESSION['client_name_profile'] = "";

                header("Location: control.php");
            }
            else
                echo "Invalid Username or Password!";
        }
    }
?>

<!DOCTYPE html>
<html lang="en" dir="ltr">
  <head>
    <meta charset="utf-8">
    <title>Login</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="css/login.css">
  </head>
  <body>

    <div class="big">
      <h1>My First Website</h1>
    </div>
   
      <div class="LoginBox">
        <div class="title">
          <h1>Login</h1>
        </div>

          <?php

            if(isset($_POST['login']))
            {
                $db = new db;
                $conn = $db->conn;
                $username = mysqli_real_escape_string($conn, $_POST['username']);
                $pass = mysqli_real_escape_string($conn, $_POST['pass']);
                $user = new Login;
                $user->checkUser($username, $pass);
            }

           ?>

        <div class="text">
          <form action="#" method="post">
            <label>Username:</label><br>
            <input type="password" name="username" placeholder="Username:"><br>
            <label>Password:</label><br>
            <input type="password" name="pass" placeholder="Password:"><br>
            <input type="submit" name="login" value="Login"><br>
          </form>
          <p>* No database yet ): SOON</p>
        </div>
        
      </div>
  </body>
</html>