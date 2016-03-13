all_tests = {}

function RegisterTest(test_function, test_name)
    test = {callback = test_function, name = test_name}
    table.insert(all_tests, test)
end

function RunAllTests()
    print('Running all tests')
    for idx, test in ipairs(all_tests) do
        local status, err = pcall(test.callback)
        if status then
            print("TEST #" .. idx, "OK",   test.name)
        else
            print("TEST #" .. idx, "FAIL", test.name, err)
        end
    end
end

function TestFail(reason)
    error(reason)
end

-- Useful functions for tests :)
-- Spawns a bot player, and returns the pointer to this player.
function SpawnBotPlayer(race, class)
    function RandomName(l)
        local length = 7
        if length < 1 then return nil end
        local array = {}
        for i = 1, length do
            array[i] = string.char(math.random(65, 90))
        end
        return table.concat(array)
    end

    -- Create bot session
    local account_id = CreateBotSession()

    -- Create character
    local player_guid = 0
    while player_guid <= 0 do
        local player_name = RandomName(7)
        coroutine.yield() -- Wait for the session to be added to the world
        player_guid = CreatePlayer(account_id, player_name, race, class)
        print(player_name)
    end

    -- Login character to bot session
    local login_status = false
    while login_status == false do
        coroutine.yield() -- Wait for the session to be added to the world
        login_status = LoginBotPlayer(account_id, player_guid)
    end

    -- Wait for player to go online
    local player = nil
    while not player do
        coroutine.yield()
        player = GetPlayerByGUID(player_guid)
    end

    return player
end

-- Testing the test framework
function test_spawnbotplayer()
    assert(SpawnBotPlayer(1, 1))
end

function test_ok()
    -- This test is always successful
end

RegisterTest(test_ok, "test_ok")
RegisterTest(test_spawnbotplayer, "test_spawnbotplayer")
