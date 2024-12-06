<?php

print "Reading BOM\n";


$dateYear = date('Y');

for ($y = $dateYear-1; $y<=$dateYear;$y++) {
	for ($w = 1; $w<=53;$w++) {
		$wpad = sprintf("%02d", $w);
		$c = file_get_contents("http://www.boxofficemojo.com/weekly/chart/?yr=".$y."&wk=".$wpad."&p=.htm");
		$content = explode("</tr>", $c);

		for ($i=0;$i<=count($content)-1; $i++) {
			
			if (preg_match('/<b>(.+?)<\/b>/', $content[$i], $out)) {
				$title = explode(" (", $out[1]);
				$title = $title[0];
				$title = str_replace(" 3-D", "", $title);
			}

			if (preg_match('/<td align="right"><font size="2">([0-9,]+?)<\/font><\/td>/', $content[$i], $out)) {
				$screens_us = str_replace(",", "", $out[1]);


				if ($title && $screens_us) {
					$o = $y-1;
					$t = $title . " (".$y.")";
					$t2 = $title . " (".$y."/I)";
					$t3 = $title . " (".$o.")";
					$t4 = $title . " (".$o."/I)";

					if (isset($movies[$t])) {
						if (isset($movies[$t]['mojo_us'])) {
							if ($screens_us > $movies[$t]['mojo_us']) {
								$movies[$t]['mojo_us'] = $screens_us;
							}
						} else {
							$movies[$t]['mojo_us'] = $screens_us;	
						}
					} else if (isset($movies[$t2])) {
						if (isset($movies[$t2]['mojo_us'])) {
							if ($screens_us > $movies[$t2]['mojo_us']) {
								$movies[$t2]['mojo_us'] = $screens_us;
							}
						} else {
							$movies[$t2]['mojo_us'] = $screens_us;	
						}
					} else if (isset($movies[$t3])) {
						if (isset($movies[$t3]['mojo_us'])) {
							if ($screens_us > $movies[$t3]['mojo_us']) {
								$movies[$t3]['mojo_us'] = $screens_us;
							}
						} else {
							$movies[$t3]['mojo_us'] = $screens_us;	
						}
					} else if (isset($movies[$t4])) {
						if (isset($movies[$t4]['mojo_us'])) {
							if ($screens_us > $movies[$t4]['mojo_us']) {
								$movies[$t4]['mojo_us'] = $screens_us;
							}
						} else {
							$movies[$t4]['mojo_us'] = $screens_us;	
						}
					} else {
						$movies[$t]['year'] = $y;
						$movies[$t]['mojo_us'] = $screens_us;
					}
				}
			}
		}

		$c = file_get_contents("http://www.boxofficemojo.com/intl/uk/?yr=".$y."&wk=".$wpad."&p=.htm");
		$content = explode("</tr>", $c);

		for ($i=0;$i<=count($content)-1; $i++) {
			
			if (preg_match('/<b>(.+?)<\/b>/', $content[$i], $out)) {
				$title = explode(" (", $out[1]);
				$title = $title[0];
				$title = str_replace(" 3-D", "", $title);
			}

			if (preg_match('/<td align="right"><font size="2">([0-9,]+?)<\/font><\/td>/', $content[$i], $out)) {
				$screens_uk = str_replace(",", "", $out[1]);

				if ($title && $screens_uk) {
					$o = $y-1;
					$t = $title . " (".$y.")";
					$t2 = $title . " (".$y."/I)";
					$t3 = $title . " (".$o.")";
					$t4 = $title . " (".$o."/I)";

					if (isset($movies[$t])) {
						if (isset($movies[$t]['mojo_uk'])) {
							if ($screens_uk > $movies[$t]['mojo_uk']) {
								$movies[$t]['mojo_uk'] = $screens_uk;
							}
						} else {
							$movies[$t]['mojo_uk'] = $screens_uk;	
						}
					} else if (isset($movies[$t2])) {
						if (isset($movies[$t2]['mojo_uk'])) {
							if ($screens_uk > $movies[$t2]['mojo_uk']) {
								$movies[$t2]['mojo_uk'] = $screens_uk;
							}
						} else {
							$movies[$t2]['mojo_uk'] = $screens_uk;	
						}
					} else if (isset($movies[$t3])) {
						if (isset($movies[$t3]['mojo_uk'])) {
							if ($screens_uk > $movies[$t3]['mojo_uk']) {
								$movies[$t3]['mojo_uk'] = $screens_uk;
							}
						} else {
							$movies[$t3]['mojo_uk'] = $screens_uk;	
						}
					} else if (isset($movies[$t4])) {
						if (isset($movies[$t4]['mojo_uk'])) {
							if ($screens_uk > $movies[$t4]['mojo_uk']) {
								$movies[$t4]['mojo_uk'] = $screens_uk;
							}
						} else {
							$movies[$t4]['mojo_uk'] = $screens_uk;	
						}
					} else {
						$movies[$t]['year'] = $y;
						$movies[$t]['mojo_uk'] = $screens_uk;
					}
				}
			}
		}
		
	}
}

$mojoUpdatedFromIndex = 0;

foreach (glob("mojo/*.mojo") as $file) {
	$file = basename($file);

	echo "file: $file\n";

	$content = file_get_contents("mojo/".$file);
	if (preg_match_all('/<td align="left" bgcolor="#.+?"><font size="2"><a href="\/movies\/\?id=(.+?)"><b>(.+?)<\/b><\/a><\/font><\/td><td bgcolor="#.+?"><font size="2">(\$.+?|n\/a)<\/font><\/td><td align="right" bgcolor="#.+?"><font size="2">(.+?)<\/font><\/td><td align="right" bgcolor="#.+?"><font size="2">(.+?)<\/font><\/td><td align="right" bgcolor="#.+?"><font size="2">(.+?)<\/font><\/td><td align="right" bgcolor="#.+?"><font size="2">(.+?)<\/font><\/td>/', $content, $mojo)) {
		
		foreach ($mojo[2] as $key => $title) {


			$title = trim($title);
			$title = preg_replace('/ \(\d\d\d\d\)/', '', $title);

			$idurl = $mojo[1][$key];
			$year = $mojo[7][$key];

			if ($year == 'TBD') {
				$year = 'N/A';
			} else {
				if (preg_match('/\d\d?\/\d\d?\/(\d\d\d\d)/', $year, $m)) {
					$year = $m[1];
				} else {
					if (preg_match('/yr=(\d\d\d\d)/', $year, $m)) {
						$year = $m[1];
					}
				}				
			}

			$mojogrosss = $mojo[3][$key];

			$mojo_us = $mojo[4][$key];
			$mojo_us = str_replace(',', '', $mojo_us);


			if ($mojo_us == 'n/a') {
				$mojo_us = 0;
			}

			$t = $title . " (".$year.")";
			$t2 = $title . " (".$year."/I)";

			if (isset($movies[$t])) {
				if (isset($movies[$t]['mojo_us'])) {
					if ($mojo_us > $movies[$t]['mojo_us']) {
						$movies[$t]['mojo_us'] = $mojo_us;
						$mojoUpdatedFromIndex++;
					}
				} else {
					$movies[$t]['mojo_us'] = $mojo_us;
					$mojoUpdatedFromIndex++;
				}
			} else if (isset($movies[$t2])) {
				if (isset($movies[$t2]['mojo_us'])) {
					if ($mojo_us > $movies[$t2]['mojo_us']) {
						$movies[$t2]['mojo_us'] = $mojo_us;
						$mojoUpdatedFromIndex++;
					}
				} else {
					$movies[$t2]['mojo_us'] = $mojo_us;
					$mojoUpdatedFromIndex++;
				}
			} else {
				continue;
			}

			$mojo_us = 0;
			$mojo_uk = 0;
			$out = array();
		}
	} else {
		echo "not matched! $file\n";
	}
}
print "BOM update from index: $mojoUpdatedFromIndex\n";


foreach ($movies as $key => $movie) {
	if (isset($movie['mojo_uk']) && isset($movie['mojo_us'])) {
		echo "TITLE " . $key . " Y " . $movie['year'] . " US " . $movie['mojo_us'] . " UK " . $movie['mojo_uk'] . "\n";	
	} else if (isset($movie['mojo_uk'])) {
		echo "TITLE " . $key . " Y " . $movie['year'] . " UK " . $movie['mojo_uk'] . "\n";	
	} else if (isset($movie['mojo_us'])) {
		echo "TITLE " . $key . " Y " . $movie['year'] . " US " . $movie['mojo_us'] . "\n";	
	}
}

print "Reading BOM finished\n";