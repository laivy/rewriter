USE [AccountDB]

IF OBJECT_ID('Register', 'P') IS NOT NULL
    DROP PROCEDURE [Register]
GO

CREATE PROCEDURE [Register]
    @ID NVARCHAR(16),
    @Password NVARCHAR(16)
AS
BEGIN
    IF EXISTS (SELECT 1 FROM [dbo].[Account] WHERE [ID] = @ID)
        RETURN 1

    INSERT INTO [dbo].[Account] VALUES (@ID, PWDENCRYPT(@Password), GETDATE())
    RETURN 0
END